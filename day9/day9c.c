#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct map
{
	char *data;
	char *visited;
	size_t width;
	size_t height;
};

static void map_release(struct map *m)
{
	free(m->data);
}

static int map_load(FILE *in, struct map *m)
{
	m->data = NULL;
	m->width = m->height = 0;

	char *line = NULL;
	size_t lsize = 0;
	ssize_t len;
	size_t mcount = 0;
	while ((len = getline(&line, &lsize, in)) != -1)
	{
		len--;
		if (!m->data)
		{
			m->width = len;
			m->data = malloc(m->width * m->width);
			if (!m->data)
			{
				return -1;
			}
		}
		if ((size_t)len != m->width)
		{
			break;
		}
		memmove(m->data + mcount, line, m->width);
		m->height++;
		mcount += m->width;
	}
	free(line);

	return 0;
}

static const int DX[] = { -1, 0, 1, 0 };
static const int DY[] = { 0, -1, 0, 1 };

static size_t map_risk(struct map *m, int x, int y)
{
	int value = m->data[y * m->width + x];
	for (int i = 0; i < 4; i++)
	{
		int nx = x + DX[i];
		int ny = y + DY[i];
		if (0 <= nx && (size_t)nx < m->width
		    && 0 <= ny && (size_t)ny < m->height)
		{
			if (m->data[ny * m->width + nx] <= value)
			{
				return 0;
			}
		}
	}
	return 1 + (value - '0');
}

static size_t map_dfs(struct map *m, int x, int y)
{
	if (m->data[y * m->width + x] == '9'
	    || m->visited[y * m->width + x])
	{
		return 0;
	}

	m->visited[y * m->width + x] = 1;
	size_t size = 1;
	for (int i = 0; i < 4; i++)
	{
		int nx = x + DX[i];
		int ny = y + DY[i];
		if (0 <= nx && (size_t)nx < m->width
		    && 0 <= ny && (size_t)ny < m->height)
		{
			size += map_dfs(m, nx, ny);
		}
	}
	return size;
}

static size_t part1(struct map *m)
{
        size_t sum = 0;
	for (size_t y = 0; y < m->height; y++)
	{
		for (size_t x = 0; x < m->width; x++)
		{
			sum += map_risk(m, x, y);
		}
	}
	return sum;
}

static int basin_cmp(const void *pa, const void *pb)
{
	const size_t *a = pa;
	const size_t *b = pb;
	return *a < *b ? 1 : *a == *b ? 0 : -1;
}

static size_t part2(struct map *m)
{
	m->visited = calloc(m->width, m->height);
	if (!m->visited)
	{
		return -1;
	}

	size_t *basin = NULL;
	size_t bcount = 0;
	size_t bsize = 0;

	for (size_t y = 0; y < m->height; y++)
	{
		for (size_t x = 0; x < m->width; x++)
		{
			size_t size = map_dfs(m, x, y);
			if (size)
			{
				if (bcount == bsize)
				{
					size_t nsize = bsize ? bsize * 2 : 128;
					size_t *nbasin = realloc(basin, nsize * sizeof(*nbasin));
					if (!nbasin)
					{
						break;
					}
					bsize = nsize;
					basin = nbasin;
				}
				basin[bcount++] = size;
			}
		}
	}

	qsort(basin, bcount, sizeof(*basin), basin_cmp);
	size_t mul = 1;
	for (size_t i = 0; i < 3 && i < bcount; i++)
	{
		mul *= basin[i];
	}
	free(basin);
	free(m->visited);
	return mul;
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

	struct map m = {0};
	int r = map_load(in, &m);
	fclose(in);
	if (r < 0)
	{
		fprintf(stderr, "Cannot parse the map\n");
		return 1;
	}

	printf("Part1: %zu\n", part1(&m));
	printf("Part2: %zu\n", part2(&m));

	map_release(&m);
	return 0;
}
