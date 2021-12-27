#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct frag_literals
{
	int a;
	int b;
	int c;
};

static char *load(FILE *in)
{
	char *data = NULL;
	size_t size = 0;
	size_t count = 0;

	char buf[4095];
	size_t len;
	while ((len = fread(buf, 1, sizeof(buf), in)))
	{
		if (count + len >= size)
		{
			size_t nsize = size ? size * 2 : sizeof(buf) + 1;
			char *ndata = realloc(data, nsize);
			if (!ndata)
			{
				break;
			}
			size = nsize;
			data = ndata;
		}
		memmove(data + count, buf, len);
		count += len;
	}
	if (data)
	{
		data[count] = 0;
	}
	return data;
}

static inline int in_array(size_t value, const size_t *a, size_t count)
{
	while (count-->0)
	{
		if (a[count] == value)
		{
			return 1;
		}
	}
	return 0;
}

static void literals(char *program, struct frag_literals *clist)
{
	char *frags[14];
	frags[0] = program;
	for (size_t i = 1; i < 14; i++)
	{
		frags[i] = strstr(frags[i-1]+3, "inp");
	}

	size_t lines[3];
	size_t lcount = 0;
	for (size_t i = 0; i < 14; i++)
	{
		for (size_t j = 0; j < i; j++)
		{
			char *fa = frags[i];
			char *fb = frags[j];
			char *ea, *eb;
			size_t row = 0;
			do
			{
				ea = strchrnul(fa, '\n');
				eb = strchrnul(fb, '\n');
				if (memcmp(fa, fb, ea-fa)
				    && !in_array(row, lines, lcount)
				    && lcount < 3)
				{
					lines[lcount++] = row;
				}
				fa = ea + 1;
				fb = eb + 1;
				row++;
			} while (ea[0] && eb[0] && memcmp(fa, "inp", 3));
		}
	}
	size_t i = 0;
	size_t row = 0;
	int c[3] = {0};
	size_t ccount = 0;
	char *cur = program;
	while (i < 14)
	{
		if (!memcmp(cur, "inp", 3) && row)
		{
			clist[i].a = c[0];
			clist[i].b = c[1];
			clist[i].c = c[2];
			i++;
			row = 0;
			ccount = 0;
		}
		else if (in_array(row, lines, lcount))
		{
			char *pos = strchr(strchr(cur, ' ')+1, ' ');
			if (pos)
			{
				c[ccount++] = strtol(pos, NULL, 10);
			}
		}
		cur = strchr(cur, '\n');
		if (!cur)
		{
			break;
		}
		cur++;
		row++;
	}
	clist[i].a = c[0];
	clist[i].b = c[1];
	clist[i].c = c[2];
}

static inline int shot(struct frag_literals *cns, int d, int z)
{
	if (z % 26 + cns->b != d)
	{
		return z / cns->a * 26 + d + cns->c;
	}
	else
	{
		return z / cns->a;
	}
}

static uint64_t min;
static uint64_t max;

static void backtrack(int *res, int k, int z, struct frag_literals *clist)
{
	if (k == 14)
	{
		if (z == 0)
		{
			min = 0;
			while (k-->0)
			{
				min = min * 10 + *res++;
			}
			if (!max)
			{
				max = min;
			}
		}
	}
	else
	{
		struct frag_literals *c = clist + k;
		for (int d = 9; d > 0; d--)
		{
			if (c->b > 0 || z % 26 + c->b == d)
			{
				res[k] = d;
				backtrack(res, k+1, shot(c, d, z), clist);
			}
		}
	}
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
		return -1;
	}

	char *program = load(in);
	fclose(in);

	struct frag_literals clist[14];
	literals(program, clist);
	free(program);

	int res[14];
	backtrack(res, 0, 0, clist);
	printf("Part1: %lu\n", max);
	printf("Part2: %lu\n", min);
	return 0;
}
