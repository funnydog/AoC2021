#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *load_file(FILE *in)
{
	char *txt = NULL;
	size_t size = 0;
	size_t count = 0;

	size_t len;
	char buf[4096];
	while ((len = fread(buf, 1, sizeof(buf), in)))
	{
		if (count + len + 1 >= size)
		{
			size_t nsize = size ? size * 2 : 4096;
			char *ntxt = realloc(txt, nsize);
			if (!ntxt)
			{
				break;
			}
			size = nsize;
			txt = ntxt;
		}
		memmove(txt + count, buf, len);
		count += len;
	}
	txt[count] = 0;
	return txt;
}

static const char *OPEN = "([{<";
static const char *CLOSED = ")]}>";
static const size_t ERRPOINTS[] = { 3, 57, 1197, 25137 };

static size_t check_line(const char *line, char *stack, size_t ssize)
{
	size_t scount = 0;
	ssize--;
	for (; *line; line++)
	{
		const char *p = strchr(OPEN, *line);
		if (p)
		{
			assert(scount < ssize);
			stack[scount++] = CLOSED[p - OPEN];
			stack[scount] = 0;
		}
		else
		{
			char c = stack[--scount];
			stack[scount] = 0;
			if (c != *line)
			{
				p = strchr(CLOSED, *line);
				return ERRPOINTS[p-CLOSED];
			}
		}
	}
	return 0;
}

static int sizecmp(const void *pa, const void *pb)
{
	const size_t *a = pa;
	const size_t *b = pb;
	return (*a < *b) ? -1 : (*a == *b) ? 0 : -1;
}

static void solve(char *txt, size_t *part1, size_t *part2)
{
	char stack[200];
	size_t s = 0;
	/*
	 * NOTE: yes, the following is a hack to avoid to allocate
	 * memory and it works only because the first string has a
	 * length > sizeof(size_t), but what the heck, this code is
	 * not going to the moon.
	 */
	size_t *comp = (size_t *)txt;
	size_t ccount = 0;

	for (char *line = strtok(txt, "\n");
	     line;
	     line = strtok(NULL, "\n"))
	{
		size_t points = check_line(line, stack, sizeof(stack));
		s += points;
		if (!points)
		{
			size_t score = 0;
			size_t slen = strlen(stack);
			while (slen-->0)
			{
				char *p = strchr(CLOSED, stack[slen]);
				score = score * 5 + (p - CLOSED) + 1;
			}
			comp[ccount++] = score;
		}
	}
	qsort(comp, ccount, sizeof(*comp), sizecmp);
	*part1 = s;
	*part2 = comp[ccount/2];
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

	char *txt = load_file(in);
	fclose(in);

	size_t part1, part2;
	solve(txt, &part1, &part2);
	printf("Part1: %zu\n", part1);
	printf("Part2: %zu\n", part2);

	free(txt);
	return 0;
}
