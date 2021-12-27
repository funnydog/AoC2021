#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct result
{
	uint64_t w1;
	uint64_t w2;
} cache[10][10][21][21];

static uint64_t part1(int p1, int p2)
{
	int s1, s2;
	s1 = s2 = 0;
	unsigned dice = 1;
	uint64_t rolls = 0;
	for (;;)
	{
		unsigned value = 0;
		for (int i = 0; i < 3; i++)
		{
			value += dice;
			if (++dice > 100)
			{
				dice = 1;
			}
		}
		rolls += 3;

		p1 = (p1 + value - 1) % 10 + 1;
		s1 += p1;
		if (s1 >= 1000)
		{
			break;
		}
		int t = p1;
		p1 = p2;
		p2 = t;
		t = s1;
		s1 = s2;
		s2 = t;
	}
	return rolls * s2;
}

static struct result winners(int p1, int p2, int s1, int s2)
{
	struct result *r = &cache[p1-1][p2-1][s1][s2];
	if (r->w1 != UINT64_MAX)
	{
		return *r;
	}

	uint64_t w1 = 0;
	uint64_t w2 = 0;
	for (int i = 1; i <= 3; i++)
	{
		for (int j = 1; j <=3; j++)
		{
			for (int k = 1; k <= 3; k++)
			{
				int np1 = (p1 + i + j + k - 1) % 10 + 1;
				int ns1 = s1 + np1;
				if (ns1 >= 21)
				{
					w1++;
				}
				else
				{
					struct result s = winners(p2, np1, s2, ns1);
					w1 += s.w2;
					w2 += s.w1;
				}
			}
		}
	}
	r->w1 = w1;
	r->w2 = w2;
	return *r;
}

static uint64_t part2(int p1, int p2)
{
	memset(cache, -1, sizeof(cache));
	struct result s = winners(p1, p2, 0, 0);
	return s.w1 > s.w2 ? s.w1 : s.w2;
}

static int load_positions(FILE *in, int *p1, int *p2)
{
	int i, p;
	while (fscanf(in, " Player %d starting position: %d", &i, &p) == 2)
	{
		switch (i)
		{
		case 1: *p1 = p; break;
		case 2: *p2 = p; break;
		default:
			return -1;
		}
	}
	return 0;
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

	int p1 = 0;
	int p2 = 0;
	load_positions(in, &p1, &p2);
	fclose(in);

	if (0 < p1 && p1 <= 10 && 0 < p2 && p2 <= 10)
	{
		printf("Part1: %lu\n", part1(p1, p2));
		printf("Part2: %lu\n", part2(p1, p2));
		return 0;
	}
	else
	{
		printf("Not in the accepted range: %d %d\n", p1, p2);
		return 1;
	}
}
