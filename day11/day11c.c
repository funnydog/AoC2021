#include <stdio.h>
#include <stdlib.h>

struct map
{
	char map[10][10];
};

static void map_load(FILE *in, struct map *m)
{
	int y = 0;
	int x = 0;
	int ch;
	while (y < 10 && (ch = fgetc(in)) != EOF)
	{
		if (ch == '\n')
		{
			y++;
			x = 0;
		}
		else if (x < 10)
		{
			m->map[y][x] = ch;
			x++;
		}
	}
}

static void map_inc(struct map *m, int x, int y)
{
	static const int DX[] = {-1, -1, 0, 1, 1, 1, 0, -1};
	static const int DY[] = {0, -1, -1, -1, 0, 1, 1, 1};

	if (m->map[y][x] && ++m->map[y][x] > '9')
	{
		m->map[y][x] = 0;
		for (int i = 0; i < 8; i++)
		{
			int nx = x + DX[i];
			int ny = y + DY[i];
			if (0 <= nx && nx < 10 && 0 <= ny && ny < 10)
			{
				map_inc(m, nx, ny);
			}
		}
	}
}

static size_t map_step(struct map *m)
{
	for (int y = 0; y < 10; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			map_inc(m, x, y);
		}
	}

	size_t count = 0;
	for (int y = 0; y < 10; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			if (m->map[y][x] == 0)
			{
				m->map[y][x] = '0';
				count++;
			}
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

	FILE *in = fopen(argv[1], "rb");
	if (!in)
	{
		fprintf(stderr, "Cannot open %s\n", argv[1]);
		return 1;
	}

	struct map m;
	map_load(in, &m);
	fclose(in);

	size_t part1 = 0;
	size_t part2 = 0;
	while (1)
	{
		size_t flashes = map_step(&m);
		if (part2 < 100)
		{
			part1 += flashes;
		}
		part2++;
		if (flashes == 100)
		{
			break;
		}
	}

	printf("Part1: %zu\n", part1);
	printf("Part2: %zu\n", part2);
	return 0;
}
