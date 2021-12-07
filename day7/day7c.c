#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

static int cost1(int d)
{
	return d;
}

static int cost2(int d)
{
	return d * (d + 1) / 2;
}

static int solve(const int *arr, size_t count, int min, int max, int(*cost)(int))
{
	int minfuel = INT_MAX;
	for (int p = min; p <= max; p++)
	{
		int sum = 0;
		for (size_t i = 0; i < count; i++)
		{
			sum += cost(abs(arr[i] - p));
		}
		if (minfuel > sum)
		{
			minfuel = sum;
		}
	}
	return minfuel;
}

static void minmax(const int *arr, size_t count, int *min, int *max)
{
	int imin = INT_MAX;
	int imax = INT_MIN;
	while (count-->0)
	{
		if (imin > arr[count])
		{
			imin = arr[count];
		}
		if (imax < arr[count])
		{
			imax = arr[count];
		}
	}
	*min = imin;
	*max = imax;
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

	int *arr = NULL;
	size_t size = 0;
	size_t count = 0;

	int tmp;
	while (fscanf(in, "%d,", &tmp) == 1)
	{
		if (count == size)
		{
			size_t nsize = size ? size * 2 : 128;
			int *narr = realloc(arr, sizeof(*narr) * nsize);
			if (!narr)
			{
				break;
			}
			size = nsize;
			arr = narr;
		}
		arr[count++] = tmp;
	}
	fclose(in);

	int min, max;
	minmax(arr, count, &min, &max);
	printf("Part1: %d\n", solve(arr, count, min, max, cost1));
	printf("Part2: %d\n", solve(arr, count, min, max, cost2));

	free(arr);
	return 0;
}
