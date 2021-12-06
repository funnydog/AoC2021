#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint64_t *load_initial(FILE *in, size_t *count)
{
	uint64_t*arr = NULL;
	size_t asize = 0;
	size_t acount = 0;

	uint64_t tmp;
	while (fscanf(in, "%" SCNu64 ",", &tmp) == 1)
	{
		if (acount == asize)
		{
			size_t nsize = asize ? asize * 2 : 64;
			uint64_t *narr = realloc(arr, nsize * sizeof(*arr));
			if (!narr)
			{
				break;
			}
			asize = nsize;
			arr = narr;
		}
		arr[acount++] = tmp;
	}
	*count = acount;
	return arr;
}

static uint64_t solve(const uint64_t *arr, size_t count, size_t days)
{
	assert(days <= 256);

	uint64_t day[256+9];
	memset(day, 0, sizeof(day));

	for (size_t i = 0; i < count; i++)
	{
		day[arr[i]] += 1;
	}

	uint64_t sum = count;
	for (size_t i = 0; i < days; i++)
	{
		sum += day[i];
		day[i + 7] += day[i];
		day[i + 9] += day[i];
	}

	return sum;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		exit(1);
	}

	FILE *in = fopen(argv[1], "rb");
	if (!in)
	{
		fprintf(stderr, "Cannot open %s\n", argv[1]);
		exit(1);
	}

	size_t count;
	uint64_t *arr = load_initial(in, &count);
	fclose(in);

	printf("Part1: %" PRIu64 "\n", solve(arr, count, 80));
	printf("Part2: %" PRIu64 "\n", solve(arr, count, 256));

	free(arr);
	return 0;
}
