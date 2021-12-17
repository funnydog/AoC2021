#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct area
{
	int x0, x1;
	int y0, y1;
};

static int reverse_gauss(int sum)
{
	return ceill((sqrt(1 + 8 * sum) -1) * 0.5);
}

static int sim(const struct area *a, int vx, int vy)
{
	int x = 0;
	int y = 0;

	/* boost the calculation if it's coming back */
	if (vy >= 0)
	{
		int t = 2 * vy + 1;
		vy = -vy - 1;
		x = (vx + 1) * vx / 2;
		if (vx > t)
		{
			vx -= t;
		}
		else
		{
			vx = 0;
		}
		x -= (vx + 1) * vx / 2;
	}

	for (;;)
	{
		x += vx;
		y += vy;
		if (vx)
		{
			vx--;
		}
		vy--;
		if (x > a->x1 || y < a->y0)
		{
			return 0;
		}
		else if (a->x0 <= x && y <= a->y1)
		{
			return 1;
		}
	}
}

static int simx(const struct area *a, int vx)
{
	int x = 0;
	for (;;)
	{
		x += vx;
		if (vx)
		{
			vx--;
		}
		if (x > a->x1)
		{
			return 0;
		}
		else if (a->x0 <= x)
		{
			return 1;
		}
		else if (vx == 0)
		{
			return 0;
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
	struct area a;
	fscanf(in, "target area: x=%d..%d, y=%d..%d",
	       &a.x0, &a.x1, &a.y0, &a.y1);
	fclose(in);

	int vy_max = (a.y0 + 1) * a.y0 / 2;
	printf("Part1: %d\n", vy_max);

	size_t count = 0;
	for (int vx = reverse_gauss(a.x0); vx <= a.x1; vx++)
	{
		if (!simx(&a, vx))
		{
			continue;
		}
		for (int vy = a.y0; vy <= vy_max; vy++)
		{
			if (sim(&a, vx, vy))
			{
				count++;
			}
		}
	}
	printf("Part2: %zu\n", count);
	return 0;
}
