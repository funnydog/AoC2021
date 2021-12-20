#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DCTINCR 53

struct dict
{
	struct delem
	{
		int x, y;
		int value;
	} *table;
	size_t count;
	size_t size;
};

static inline size_t hashfn(int x, int y)
{
	return 5381 + x * 101 + y;
}

static void dict_rehash(struct dict *d)
{
	size_t size = d->size ? d->size * 2 : 128;
	struct delem *table = malloc(size * sizeof(*table));
	if (!table)
	{
		abort();
	}
	for (size_t i = 0; i < size; i++)
	{
		table[i].x = INT_MAX;
	}
	for (size_t i = 0; i < d->size; i++)
	{
		if (d->table[i].x == INT_MAX)
		{
			continue;
		}

		size_t idx = hashfn(d->table[i].x, d->table[i].y) % size;
		while (table[idx].x != INT_MAX)
		{
			idx += DCTINCR;
			if (idx >= size)
			{
				idx -= size;
			}
		}
		table[idx] = d->table[i];
	}
	free(d->table);
	d->table = table;
	d->size = size;
}

static void dict_clear(struct dict *d)
{
	for (size_t i = 0; i < d->size; i++)
	{
		d->table[i].x = INT_MAX;
	}
	d->count = 0;
}

static void dict_set(struct dict *d, int x, int y, int value)
{
	if (d->count * 100 >= d->size * 75)
	{
		dict_rehash(d);
	}
	size_t idx = hashfn(x, y) % d->size;
	while (d->table[idx].x != INT_MAX)
	{
		if (d->table[idx].x == x && d->table[idx].y == y)
		{
			break;
		}
		idx += DCTINCR;
		if (idx >= d->size)
		{
			idx -= d->size;
		}
	}
	if (d->table[idx].x == INT_MAX)
	{
		d->table[idx].x = x;
		d->table[idx].y = y;
		d->table[idx].value = value;
		d->count++;
	}
}

static int dict_get(const struct dict *d, int x, int y, int def)
{
	size_t idx = hashfn(x, y) % d->size;
	while (d->table[idx].x != INT_MAX)
	{
		if (d->table[idx].x == x && d->table[idx].y == y)
		{
			return d->table[idx].value;
		}
		idx += DCTINCR;
		if (idx >= d->size)
		{
			idx -= d->size;
		}
	}
	return def;
}

struct map
{
	struct dict data;
	struct dict swap;
	int bg;
	int xmin, xmax;
	int ymin, ymax;
};

static void map_release(struct map *m)
{
	free(m->data.table);
	free(m->swap.table);
}

static void load_map(FILE *in, struct map *m)
{
	dict_clear(&m->data);
	m->bg = '.';
	m->xmin = m->ymin = INT_MAX;
	m->xmax = m->ymax = INT_MIN;

	int ch;
	int x, y;
	x = y = 0;
	while ((ch = getc(in)) != EOF)
	{
		switch (ch)
		{
		case '#':
			dict_set(&m->data, x, y, '#');
			if (m->xmin > x) m->xmin = x;
			if (m->xmax < x) m->xmax = x;
			if (m->ymin > y) m->ymin = y;
			if (m->ymax < y) m->ymax = y;

		case '.':
			x++;
			break;

		case '\n':
			x = 0;
			y++;
			break;

		default:
			break;
		}
	}
}

static void map_enhance(struct map *m, const char *algo)
{
	int nbg = m->bg == '#' ? algo[511] : algo[0];
	int xmin, xmax, ymin, ymax;
	xmin = ymin = INT_MAX;
	xmax = ymax = INT_MIN;

	dict_clear(&m->swap);
	for (int y = m->ymin - 2; y <= m->ymax + 2; y++)
	{
		for (int x = m->xmin - 2; x <= m->xmax + 2; x++)
		{
			unsigned idx = 0;
			for (int ny = y-1; ny <= y + 1; ny++)
			{
				for (int nx = x-1; nx <= x + 1; nx++)
				{
					idx <<= 1;
					if (dict_get(&m->data, nx, ny, m->bg) == '#')
					{
						idx |= 1;
					}
				}
			}

			if (algo[idx] != nbg)
			{
				dict_set(&m->swap, x, y, algo[idx]);
				if (xmin > x) xmin = x;
				if (xmax < x) xmax = x;
				if (ymin > y) ymin = y;
				if (ymax < y) ymax = y;
			}
		}
	}
	struct dict t = m->data;
	m->data = m->swap;
	m->swap = t;

	m->bg = nbg;
	m->xmin = xmin;
	m->xmax = xmax;
	m->ymin = ymin;
	m->ymax = ymax;
}

static void load_algo(FILE *in, char *algo)
{
	size_t i = 0;
	for (;;)
	{
		int ch = getc(in);
		if (ch == EOF || ch == '\n')
		{
			break;
		}
		else if (ch == '#' || ch == '.')
		{
			if (i < 512)
			{
				algo[i++] = ch;
			}
		}
		else
		{
		}
	}
	getc(in);
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

	char algo[512];
	load_algo(in, algo);

	struct map m = {0};
	load_map(in, &m);
	fclose(in);

	for (int i = 0; i < 2; i++)
	{
		map_enhance(&m, algo);
	}
	printf("Part1: %zu\n", m.data.count);
	for (int i = 0; i < 48; i++)
	{
		map_enhance(&m, algo);
	}
	printf("Part2: %zu\n", m.data.count);

	map_release(&m);
	return 0;
}
