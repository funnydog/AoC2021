#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

struct report
{
	unsigned *data;
	size_t count;
	size_t size;
	size_t bitlen;
};

static void report_release(struct report *r)
{
	free(r->data);
}

static void report_load(FILE *in, struct report *r)
{
	r->count = 0;
	r->bitlen = 0;
	unsigned value = 0;
	int ch;
	while ((ch = fgetc(in)) != EOF)
	{
		switch (ch)
		{
		case '0':
		case '1':
			value = value << 1 | (ch - '0');
			if (!r->count)
			{
				r->bitlen++;
			}
			break;

		case '\n':
			if (r->count == r->size)
			{
				size_t size = r->size ? r->size * 2 : 8;
				unsigned *data = realloc(r->data, size * sizeof(*data));
				if (!data)
				{
					return;
				}
				r->size = size;
				r->data = data;
			}
			r->data[r->count] = value;
			r->count++;
			value = 0;
			break;

		default:
			break;
		}
	}
}

static unsigned part1(struct report *r)
{
	unsigned gamma = 0;
	unsigned epsilon = 0;
	size_t k = r->bitlen;
	while (k-- > 0)
	{
		size_t ones = 0;
		for (size_t i = 0; i < r->count; i++)
		{
			ones += !!(r->data[i] & 1U<<k);
		}
		gamma <<= 1;
		epsilon <<= 1;
		if (ones * 2 > r->count)
		{
			gamma |= 1;
		}
		else
		{
			epsilon |= 1;
		}
	}
	return gamma * epsilon;
}

static unsigned filter(struct report *r, int o2)
{
	unsigned mask = 0;
	unsigned value = 0;
	size_t k = r->bitlen;
	while (k-->0)
	{
		size_t zeros = 0;
		size_t count = 0;
		unsigned last = 0;
		for (size_t i = 0; i < r->count; i++)
		{
			if ((r->data[i] & mask) == value)
			{
				last = r->data[i];
				zeros += !(last & 1U<<k);
				count++;
			}
		}
		if (count == 1)
		{
			return last;
		}
		mask |= 1U << k;
		value |= o2
			? (zeros * 2 <= count)<<k
			: (zeros * 2 > count)<<k;
	}
	return value;
}

static unsigned part2(struct report *r)
{
	return filter(r, 1) * filter(r, 0);
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		return 1;
	}

	FILE *in = fopen(argv[1], "rb");
	if  (!in)
	{
		fprintf(stderr, "Cannot open %s\n", argv[1]);
		return 1;
	}

	struct report r = {0};
	report_load(in, &r);
	fclose(in);

	printf("Part1: %u\n", part1(&r));
	printf("Part2: %u\n", part2(&r));

	report_release(&r);
	return 0;
}
