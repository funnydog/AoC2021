#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

static size_t solve(const int *depths, size_t acount, size_t l)
{
	size_t count = 0;
	if (acount >= l)
	{
		int sum = 0;
		size_t i = 0;
		for (; i < l; i++)
		{
			sum += depths[i];
		}
		for (; i < acount; i++)
		{
			int nsum = sum + depths[i] - depths[i-l];
			if (sum < nsum)
			{
				count++;
			}
			sum = nsum;
		}
	}
	return count;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		return 1;
	}

	FILE *input = fopen(argv[1], "rb");
	if (!input)
	{
		fprintf(stderr, "Cannot open %s\n", argv[1]);
		return 1;
	}

	int *depths = NULL;
	size_t acount = 0;
	size_t asize = 0;

	int depth;
	while (fscanf(input, "%d", &depth) == 1)
	{
		if (acount == asize)
		{
			size_t nsize = asize ? asize * 2 : 128;
			int *ndepths = realloc(depths, nsize * sizeof(*ndepths));
			if (!ndepths)
			{
				break;
			}
			asize = nsize;
			depths = ndepths;
		}
		depths[acount++] = depth;
	}
	fclose(input);

	printf("Part1: %zu\n", solve(depths, acount, 1));
	printf("Part2: %zu\n", solve(depths, acount, 3));
	free(depths);

	return 0;
}
