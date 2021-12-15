#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct map
{
	int *data;
	size_t width;
	size_t height;
};

struct helem
{
	int prio;
	size_t x;
	size_t y;
};

struct heap
{
	struct helem *data;
	size_t count;
	size_t size;
};

static void bubble_up(struct heap *h, size_t pos)
{
	while (pos > 0)
	{
		size_t parent = (pos - 1) / 2;

		/* exit if the dominance relation is satisfied */
		if (h->data[pos].prio > h->data[parent].prio)
		{
			break;
		}

		/* swap the with the parent */
		struct helem t = h->data[parent];
		h->data[parent] = h->data[pos];
		h->data[pos] = t;

		/* check again the parent */
		pos = parent;
	}
}

static void bubble_down(struct heap *h, size_t pos)
{
	for (;;)
	{
		/* find the lowest prio between children and parent */
		size_t min = pos;
		for (size_t j = pos * 2 + 1; j <= pos * 2 + 2; j++)
		{
			if (j < h->count && h->data[min].prio > h->data[j].prio)
			{
				min = j;
			}
		}

		/*
		 * exit if the dominance relation is satisfied,
		 * i.e. the parent has the lowest priority
		 */
		if (min == pos)
		{
			break;
		}

		/*
		 * swap the parent with the child with the lowest
		 * priority
		 */
		struct helem t = h->data[pos];
		h->data[pos] = h->data[min];
		h->data[min] = t;

		/* check again the lowest-priority child */
		pos = min;
	}
}

static void heap_push(struct heap *h, int prio, size_t x, size_t y)
{
	if (h->count == h->size)
	{
		size_t nsize = h->size ? h->size * 2 : 32;
		struct helem *ndata = realloc(h->data, nsize * sizeof(*ndata));
		if (!ndata)
		{
			abort();
		}
		h->size = nsize;
		h->data = ndata;
	}
	struct helem *e = h->data + h->count;
	e->prio = prio;
	e->x = x;
	e->y = y;
	bubble_up(h, h->count);
	h->count++;
}

static void heap_pop(struct heap *h, int *prio, size_t *x, size_t *y)
{
	assert(h->count);
	*prio = h->data[0].prio;
	*x = h->data[0].x;
	*y = h->data[0].y;
	h->count--;

	h->data[0] = h->data[h->count];
	bubble_down(h, 0);
}

static void heap_release(struct heap *h)
{
	free(h->data);
}

static void map_load(FILE *in, struct map *m)
{
	free(m->data);
	m->data = NULL;
	m->width = m->height = 0;

	size_t count = 0;
	size_t size = 0;

	int ch;
	while ((ch = fgetc(in)) != EOF)
	{
		if (ch == '\n')
		{
			m->height++;
		}
		if ('1' <= ch && ch <= '9')
		{
			if (!m->height)
			{
				m->width++;
			}
			if (count == size)
			{
				size_t nsize = size ? size * 2 : 128;
				int *data = realloc(m->data, nsize * sizeof(*data));
				if (!data)
				{
					break;
				}
				size = nsize;
				m->data = data;
			}
			m->data[count++] = ch - '0';
		}
	}
}

static size_t map_dijkstra(struct map *m, size_t x, size_t y)
{
	static const int DX[] = {-1, 0, 1, 0};
	static const int DY[] = {0, -1, 0, 1};

	int *riskmap = calloc(m->width * m->height, sizeof(*riskmap));
	assert(riskmap);
	struct heap h = {0};
	heap_push(&h, 0, x, y);
	while (h.count)
	{
		int risk;
		heap_pop(&h, &risk, &x, &y);
		for (size_t i = 0; i < 4; i++)
		{
			size_t nx = x + DX[i];
			size_t ny = y + DY[i];
			if (nx < m->width && ny < m->height)
			{
				size_t off = ny * m->width + nx;
				int nrisk = risk + m->data[off];
				if (riskmap[off] == 0 || nrisk < riskmap[off])
				{
					riskmap[off] = nrisk;
					heap_push(&h, nrisk, nx, ny);
				}
			}
		}
	}
	heap_release(&h);

	size_t value = riskmap[m->width * m->height - 1];
	free(riskmap);

	return value;
}

static int map_extend(struct map *m, int n)
{
	size_t nwidth = m->width * n;
	size_t nheight = m->height * n;
	int *nmap = malloc(nwidth * nheight * sizeof(*nmap));
	if (!nmap)
	{
		return -1;
	}

	int *cur = nmap;
	for (int ry = 0; ry < n; ry++)
	{
		for (size_t y = 0; y < m->height; y++)
		{
			for (int rx = 0; rx < n; rx++)
			{
				for (size_t x = 0; x < m->width; x++)
				{
					*cur++ = (m->data[y * m->width + x] + rx + ry - 1) % 9 + 1;
				}
			}
		}
	}
	m->width = nwidth;
	m->height = nheight;
	free(m->data);
	m->data = nmap;
	return 0;
}

static void map_release(struct map *m)
{
	free(m->data);
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
	fclose(in);

	printf("Part1: %zu\n", map_dijkstra(&m, 0, 0));
	map_extend(&m, 5);
	printf("Part2: %zu\n", map_dijkstra(&m, 0, 0));

	map_release(&m);
	return 0;
}
