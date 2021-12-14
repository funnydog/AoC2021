#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct entry
{
	unsigned wiring[10];
	unsigned display[4];

	struct entry *next;
};

static unsigned encode(const char *str)
{
	unsigned value = 0;
	for (; *str; str++)
	{
		value |= 1U << (*str-'a');
	}
	return value;
}

static struct entry *load_entries(FILE *in)
{
	struct entry *lst = NULL;
	struct entry **tail = &lst;

	char *line = NULL;
	size_t lsize = 0;
	while (getline(&line, &lsize, in) != -1)
	{
		struct entry *e = malloc(sizeof(*e));
		if (!e)
		{
			break;
		}

		size_t i;
		char *tok;
		for (i = 0, tok = strtok(line, " \t\r\n");
		     tok && i < 10;
		     tok = strtok(NULL, " \t\r\n"), i++)
		{
			e->wiring[i] = encode(tok);
		}

		if (!tok || strcmp(tok, "|"))
		{
			free(e);
			break;
		}

		for (i = 0, tok = strtok(NULL, " \t\r\n");
		     tok && i < 4;
		     tok = strtok(NULL, " \t\r\n"), i++)
		{
			e->display[i] = encode(tok);
		}
		e->next = NULL;

		*tail = e;
		tail = &e->next;
	}
	free(line);

	return lst;
}

static void free_entries(struct entry *lst)
{
	while (lst)
	{
		struct entry *next = lst->next;
		free(lst);
		lst = next;
	}
}

static int bitcount(unsigned x)
{
	int count = 0;
	while (x)
	{
		x &= x-1;
		count++;
	}
	return count;
}

/* candidate digits for each number of segments turned on */
static struct candidates
{
	size_t count;
	unsigned digit[10];
} candidates[8];

/* number of common segments between two digits */
static int inters[10][10];

static size_t part1(struct entry *lst)
{
	size_t count = 0;
	for (; lst; lst = lst->next)
	{
		for (size_t i = 0; i < 4; i++)
		{
			if (candidates[bitcount(lst->display[i])].count == 1)
			{
				count++;
			}
		}
	}
	return count;
}

struct digit
{
	int digit;
	unsigned segments[10];
	size_t count;
};

static int digit_cmp(const void *pa, const void *pb)
{
	const struct digit *a = pa;
	const struct digit *b = pb;
	return (a->count < b->count) ? -1 : (a->count == b->count) ? 0 : 1;
}

static size_t solve(const struct entry *e)
{
	/* find the potential candidates for each digit */
	struct digit digit[10] = {0};
	for (size_t i = 0; i < 10; i++)
	{
		digit[i].digit = i;

		struct candidates *c = candidates + bitcount(e->wiring[i]);
		for (size_t j = 0; j < c->count; j++)
		{
			struct digit *d = digit + c->digit[j];
			d->segments[d->count++] = e->wiring[i];
		}
	}

	/* sort by possibility length */
	qsort(digit, 10, sizeof(digit[0]), digit_cmp);
	size_t t = 0;
	while (digit[t].count <= 1)
	{
		t++;
	}

	/* remove the wrong candidates */
	for (size_t i = 0; i < t; i++)
	{
		unsigned a = digit[i].digit;
		unsigned aseg = digit[i].segments[0];

		for (size_t j = t; j < 10; j++)
		{
			unsigned b = digit[j].digit;
			size_t k = digit[j].count;
			while (k-->0)
			{
				if (bitcount(aseg & digit[j].segments[k]) != inters[a][b])
				{
					digit[j].count--;
					digit[j].segments[k] = digit[j].segments[
						digit[j].count];
				}
			}
			if (digit[j].count <= 1)
			{
				struct digit tmp = digit[t];
				digit[t] = digit[j];
				digit[j] = tmp;
				t++;
			}
		}
	}

	if (t < 10)
	{
		fprintf(stderr, "Couldn't find a solution\n");
		return 0;
	}

	/* map the encoded value to the digit */
	unsigned map[1U<<8];
	for (size_t i = 0; i < 10; i++)
	{
		map[digit[i].segments[0]] = digit[i].digit;
	}

	/* map the display values to actual digits */
	size_t value = 0;
	for (size_t i = 0; i < 4; i++)
	{
		value = value * 10 + map[e->display[i]];
	}
	return value;
}

static size_t part2(const struct entry *lst)
{
	size_t sum = 0;
	for (; lst; lst = lst->next)
	{
		sum += solve(lst);
	}
	return sum;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		return 1;
	}

	FILE *in = fopen(argv[1], "rb");
	if (!in)
	{
		fprintf(stderr, "Cannot open %s\n", argv[1]);
		return 1;
	}

	/* description of the segments */
	static const char *segs[] = {
		"abcefg",
		"cf",
		"acdeg",
		"acdfg",
		"bcdf",
		"abdfg",
		"abdefg",
		"acf",
		"abcdefg",
		"abcdfg",
	};
	for (size_t i = 0; i < 10; i++)
	{
		struct candidates *c = &candidates[strlen(segs[i])];
		c->digit[c->count] = i;
		c->count++;

		for (size_t j = 0; j < 10; j++)
		{
			inters[i][j] = bitcount(encode(segs[i]) & encode(segs[j]));
		}
	}

	/* prepare the data */
	struct entry *lst = load_entries(in);
	fclose(in);

	printf("Part1: %zu\n", part1(lst));
	printf("Part2: %zu\n", part2(lst));

	free_entries(lst);
	return 0;
}
