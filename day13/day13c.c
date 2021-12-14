#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct dot
{
	int x;
	int y;
};

struct map
{
	struct dot *data;
	size_t count;
	size_t size;
};

static void map_release(struct map *m)
{
	free(m->data);
}

static void map_load(FILE *in, struct map *m)
{
	m->count = 0;

	char *line = NULL;
	size_t lsize = 0;
	while (getline(&line, &lsize, in) != -1)
	{
		if (line[0] == '\n')
		{
			break;
		}
		char *sx = strtok(line, ",\r\n");
		char *sy = strtok(NULL, "\r\n");
		if (!sx || !sy)
		{
			break;
		}
		if (m->count == m->size)
		{
			size_t size = m->size ? m->size * 2 : 32;
			struct dot *data = realloc(m->data, size * sizeof(*data));
			if (!data)
			{
				break;
			}
			m->size = size;
			m->data = data;
		}
		m->data[m->count].x = strtol(sx, NULL, 10);
		m->data[m->count].y = strtol(sy, NULL, 10);
		m->count++;
	}
	free(line);
}

static void map_fold_up(struct map *m, int fy)
{
	for (size_t i = 0; i < m->count; i++)
	{
		if (m->data[i].y > fy)
		{
			m->data[i].y = 2 * fy - m->data[i].y;
		}
	}
}

static void map_fold_left(struct map *m, int fx)
{
	for (size_t i = 0; i < m->count; i++)
	{
		if (m->data[i].x > fx)
		{
			m->data[i].x = 2 * fx - m->data[i].x;
		}
	}
}

static int dotcmp(const void *pa, const void *pb)
{
	const struct dot *a = pa;
	const struct dot *b = pb;
	if (a->y != b->y)
	{
		return a->y < b->y ? -1 : 1;
	}
	else if (a->x != b->x)
	{
		return a->x < b->x ? -1 : 1;
	}
	return 0;
}

static size_t map_count(struct map *m)
{
 	qsort(m->data, m->count, sizeof(m->data[0]), dotcmp);

	size_t count = 0;
	struct dot prev = {-1, -1};
	for (size_t i = 0; i < m->count; i++)
	{
		if (prev.x != m->data[i].x || prev.y != m->data[i].y)
		{
			count++;
		}
		prev = m->data[i];
	}
	return count;
}

static void map_print(struct map *m)
{
 	qsort(m->data, m->count, sizeof(m->data[0]), dotcmp);

	int px = 0;
	int py = 0;
	for (size_t i = 0; i < m->count; i++)
	{
		if (py == m->data[i].y && px > m->data[i].x)
		{
			continue;
		}
		for (; py < m->data[i].y; py++)
		{
			putchar('\n');
			px = 0;
		}
		for (; px < m->data[i].x; px++)
		{
			putchar(' ');
		}
		putchar('#');
		px++;
	}
	putchar('\n');
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

	struct map m = {0};
	map_load(in, &m);

	size_t part1 = SIZE_MAX;
	char coord;
	int pos;
	while (fscanf(in, " fold along %c=%d", &coord, &pos) == 2)
	{
		switch (coord)
		{
		case 'y': map_fold_up(&m, pos); break;
		case 'x': map_fold_left(&m, pos); break;
		}
		if (part1 == SIZE_MAX)
		{
			part1 = map_count(&m);
		}
	}
	fclose(in);

	printf("Part1: %zu\n", part1);
	printf("Part2:\n");
	map_print(&m);

	map_release(&m);

	return 0;
}
