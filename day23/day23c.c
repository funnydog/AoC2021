#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const unsigned ENERGY[] = {1, 10, 100, 1000};

struct point
{
	int x;
	int y;
};

static struct point points[11+4*4];

struct map
{
	char data[7][13];
	unsigned energy;
	struct map *next;
};

struct hash
{
	struct map *table;
	size_t count;
	size_t size;
};

struct heap
{
	struct helem
	{
		int prio;
		struct map *data;
	} *table;

	size_t count;
	size_t size;
};

static struct map *freelist;

static void map_build_points(const struct map *m, struct point *p)
{
	memset(p, 0, 27 * sizeof(*p));
	for (int y = 0; y < 7; y++)
	{
		for (int x = 0; x < 13; x++)
		{
			if (y == 1 && (x == 3 || x == 5 || x == 7 || x == 9))
			{
				continue;
			}
			int e = m->data[y][x];
			if (('A' <= e && e <= 'D') || e == '.')
			{
				p->x = x;
				p->y = y;
				p++;
			}
		}
	}
}

static struct map *map_load(FILE *in)
{
	struct map *m = malloc(sizeof(*m));
	if (!m)
	{
		return NULL;
	}
	memset(m->data, '#', sizeof(m->data));
	m->energy = 0;
	m->next = NULL;

	int ch;
	int x = 0;
	int y = 0;
	while ((ch = getc(in)) != EOF && y < 7)
	{
		switch (ch)
		{
		case '.':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
			if (x < 13)
			{
				m->data[y][x] = ch;
			}
		case ' ':
		case '#':
			x++;
			break;
		case '\n':
			y++;
			x = 0;
		default:
			break;
		}
	}
	return m;
}

static void map_insert_at(struct map *m, int y, const char *txt)
{
	if (2 <= y && y < 6)
	{
		size_t len = strlen(txt);
		if (len > 13)
		{
			len = 13;
		}
		memmove(m->data[y+1], m->data[y], (7 - y - 1) * 13);
		memmove(m->data[y], txt, len);
	}
}

static void map_free(struct map *m)
{
	if (m)
	{
		m->next = freelist;
		freelist = m;
	}
}

static struct map *map_clone(const struct map *src, struct map *next)
{
	struct map *n;
	if (freelist)
	{
		n = freelist;
		freelist = n->next;
	}
	else if (!(n = malloc(sizeof(*n))))
	{
		return next;
	}

	*n = *src;
	n->next = next;
	return n;
}

static int map_complete(const struct map *m)
{
	for (int y = 2; y < 7; y++)
	{
		for (int i = 0; i < 4; i++)
		{
			if (m->data[y][i * 2 + 3] == '#')
			{
				return 1;
			}
			if (m->data[y][i * 2 + 3] != 'A' + i)
			{
				return 0;
			}
		}
	}
	return 1;
}

static size_t hashfn(const struct map *m)
{
	size_t hash = 5381;
	for (struct point *p = points; p->x != 0; p++)
	{
		hash = (hash * 33) ^ m->data[p->y][p->x];
	}
	return hash;
}

static size_t hash_index(struct hash *h, const struct map *key)
{
	size_t idx = hashfn(key) & (h->size-1);
	while (h->table[idx].energy != UINT_MAX)
	{
		if (!memcmp(h->table[idx].data, key->data, 13 * 5))
		{
			break;
		}
		if (++idx >= h->size)
		{
			idx -= h->size;
		}
	}
	return idx;
}

static struct map *hash_get(struct hash *h, const struct map *key)
{
	if (!h->size)
	{
		return NULL;
	}
	size_t idx = hash_index(h, key);
	if (h->table[idx].energy == UINT_MAX)
	{
		return NULL;
	}
	return &h->table[idx];
}

static void hash_clear(struct hash *h)
{
	for (size_t i = 0; i < h->size; i++)
	{
		h->table[i].energy = UINT_MAX;
	}
	h->count = 0;
}

static void hash_rehash(struct hash *h)
{
	size_t osize = h->size;
	struct map *otable = h->table;

	h->size = h->size ? h->size * 2 : 256;
	h->table = calloc(h->size, sizeof(*h->table));
	if (!h->table)
	{
		abort();
	}
	hash_clear(h);
	for (size_t i = 0; i < osize; i++)
	{
		if (otable[i].energy != UINT_MAX)
		{
			size_t idx = hash_index(h, otable+i);
			h->table[idx] = otable[i];
			h->count++;
		}
	}
	free(otable);
}

static void hash_set(struct hash *h, const struct map *key)
{
	if (h->count * 100 >= h->size * 75)
	{
		hash_rehash(h);
	}

	size_t idx = hash_index(h, key);
	if (h->table[idx].energy == UINT_MAX)
	{
		h->count++;
	}
	h->table[idx] = *key;
}

static void hash_release(struct hash *h)
{
	free(h->table);
}

static inline int heap_empty(const struct heap *h)
{
	return !h->count;
}

static void heap_clear(struct heap *h)
{
	for (size_t i = 0; i < h->count; i++)
	{
		map_free(h->table[i].data);
	}
	h->count = 0;
}

static void heap_release(struct heap *h)
{
	heap_clear(h);
	free(h->table);
}

static void bubble_up(struct heap *h, size_t pos)
{
	while (pos > 0)
	{
		size_t parent = (pos - 1) / 2;

		/* dominance relation satisfied */
		if (h->table[pos].prio > h->table[parent].prio)
		{
			break;
		}

		/* swap the elements */
		struct helem tmp = h->table[parent];
		h->table[parent] = h->table[pos];
		h->table[pos] = tmp;

		/* check the parent */
		pos = parent;
	}
}

static void heap_push(struct heap *h, int prio, struct map *m)
{
	if (h->count == h->size)
	{
		size_t nsize = h->size ? h->size * 2 : 128;
		struct helem *ntable = realloc(h->table, nsize * sizeof(*ntable));
		if (!ntable)
		{
			abort();
		}
		h->size = nsize;
		h->table = ntable;
	}
	h->table[h->count].prio = prio;
	h->table[h->count].data = m;
	bubble_up(h, h->count);
	h->count++;
}

static void bubble_down(struct heap *h, size_t pos)
{
	for (;;)
	{
		/* find the lower element between parent and children */
		size_t min = pos;
		for (size_t i = pos * 2 + 1; i <= pos * 2 + 2 && i < h->count; i++)
		{
			if (h->table[min].prio > h->table[i].prio)
			{
				min = i;
			}
		}

		/* dominance relation satisfied */
		if (min == pos)
		{
			break;
		}

		/* swap the elements */
		struct helem tmp = h->table[pos];
		h->table[pos] = h->table[min];
		h->table[min] = tmp;

		/* check the child */
		pos = min;
	}
}

static struct helem heap_pop(struct heap *h)
{
	struct helem r = h->table[0];
	h->count--;
	h->table[0] = h->table[h->count];
	bubble_down(h, 0);
	return r;
}

static int map_can_move(const struct map *m, struct point p)
{
	if (p.y == 1)
	{
		return 1;
	}
	if (m->data[p.y-1][p.x] != '.')
	{
		return 0;
	}
	int el = m->data[p.y][p.x];
	if (p.x != 3 + 2 * (el - 'A'))
	{
		return 1;
	}
	for (int y = 5; y > p.y; y--)
	{
		if (m->data[y][p.x] == '#')
		{
		}
		else if (m->data[y][p.x] != el)
		{
			return 1;
		}
	}
	return 0;
}

static void find_home(const struct map *m, int e, struct point p, struct point *target)
{
	int x1 = (e - 'A') * 2 + 3;

	/* walk horizontally */
	int dx = p.x < x1 ? 1 : -1;
	while (p.x != x1)
	{
		p.x += dx;
		if (m->data[p.y][p.x] != '.')
		{
			target->x = 0;
			return;
		}
	}

	/* check vertically */
	for (p.y = 5; p.y >= 2; p.y--)
	{
		if (m->data[p.y][p.x] == '#')
		{
			continue;
		}
		if (m->data[p.y][p.x] == '.')
		{
			*target++ = p;
			break;
		}
		else if (m->data[p.y][p.x] != e)
		{
			break;
		}
	}
	target->x = 0;
}

static void find_hallways(struct map *m, struct point p, struct point *target)
{
	/* walk vertically */
	for (p.y--; p.y > 1; p.y--)
	{
		if (m->data[p.y][p.x] != '.')
		{
			target->x = 0;
			return;
		}
	}

      	/* walk left */
	int x0 = p.x;
	for (p.x = x0 - 1; ; p.x--)
	{
		if (m->data[p.y][p.x] != '.')
		{
			break;
		}
		else if (p.x != 3 && p.x != 5 && p.x != 7 && p.x != 9)
		{
			*target++ = p;
		}
	}

	/* walk right */
	for (p.x = x0 + 1; ; p.x++)
	{
		if (m->data[p.y][p.x] != '.')
		{
			break;
		}
		else if (p.x != 3 && p.x != 5 && p.x != 7 && p.x != 9)
		{
			*target++ = p;
		}
	}
	target->x = 0;
}

static inline unsigned manhattan(struct point a, struct point b)
{
	return ((a.x > b.x) ? (a.x - b.x) : (b.x - a.x))
		+ ((a.y > b.y) ? (a.y - b.y) : (b.y - a.y));
}

static struct map *transitions(struct map *m)
{
	struct map *lst = NULL;
	struct point target[16];
	for (struct point *p = points; p->x != 0; p++)
	{
		int e = m->data[p->y][p->x];
		if ('A' <= e && e <= 'D' && map_can_move(m, *p))
		{
			if (p->y == 1)
			{
				find_home(m, e, *p, target);
			}
			else
			{
				find_hallways(m, *p, target);
			}

			for (size_t i = 0; i < 16 && target[i].x; i++)
			{
				lst = map_clone(m, lst);
				lst->data[p->y][p->x] = '.';
				lst->data[target[i].y][target[i].x] = e;
				lst->energy += manhattan(*p, target[i]) * ENERGY[e - 'A'];
			}
		}
	}

	return lst;
}

static int map_prio(const struct map *m)
{
	int prio = m->energy;
	for (struct point *p = points; p->x != 0; p++)
	{
		int e = m->data[p->y][p->x];
		if ('A' <= e && e <= 'D')
		{
			int x = 3 + 2 * (e - 'A');
			x = x > p->x ? (x-p->x) : (p->x - x);
			prio += x * ENERGY[e - 'A'];
		}
	}
	return prio;
}

static size_t astar(struct map *start, struct hash *visited, struct heap *queue)
{
	size_t result = 0;

	map_build_points(start, points);
	hash_clear(visited);
	heap_clear(queue);

	hash_set(visited, start);
	heap_push(queue, 0, map_clone(start, NULL));
	while (!heap_empty(queue))
	{
		struct helem el = heap_pop(queue);
		if (map_complete(el.data))
		{
			result = el.data->energy;
			map_free(el.data);
			break;
		}
		for (struct map *n = transitions(el.data); n; )
		{
			struct map *next = n->next;
			struct map *f = hash_get(visited, n);
			if (!f || n->energy < f->energy)
			{
				hash_set(visited, n);
				heap_push(queue, map_prio(n), n);
			}
			else
			{
				map_free(n);
			}
			n = next;
		}
		map_free(el.data);
	}
	return result;
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
	struct map *m = map_load(in);
	fclose(in);
	if (m)
	{
		struct hash visited = {0};
		struct heap queue = {0};

		printf("Part1: %zu\n", astar(m, &visited, &queue));

		map_insert_at(m, 3, "  #D#C#B#A");
		map_insert_at(m, 4, "  #D#B#A#C");
		printf("Part2: %zu\n", astar(m, &visited, &queue));

		heap_release(&queue);
		hash_release(&visited);
		free(m);
	}

	while (freelist)
	{
		struct map *next = freelist->next;
		free(freelist);
		freelist = next;
	}
	return 0;
}
