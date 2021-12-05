#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

struct segment
{
	int x0;
	int y0;
	int x1;
	int y1;

	struct segment *next;
};

struct sparse_map
{
	struct vortex
	{
		int x, y;
		size_t count;
	} *table;

	size_t count;
	size_t size;
};

static void sparse_map_reset(struct sparse_map *map)
{
	for (size_t i = 0; i < map->size; i++)
	{
		map->table[i].x = INT_MAX;
	}
}

static size_t hashfn(int x, int y)
{
	return x + (y<<8);
}

static size_t sparse_map_index(struct sparse_map *map, int x, int y)
{
	size_t idx = hashfn(x, y) % map->size;
	while (map->table[idx].x != INT_MAX)
	{
		if (map->table[idx].x == x && map->table[idx].y == y)
		{
			break;
		}
		if ((idx += 13) >= map->size)
		{
			idx -= map->size;
		}
	}
	return idx;
}

static int sparse_map_rehash(struct sparse_map *map)
{
	struct vortex *old = map->table;
	size_t oldsize = map->size;

	map->size = map->size ? map->size * 2 : 128;
	map->table = malloc(map->size * sizeof(*map->table));
	if (!map->table)
	{
		return -1;
	}

	sparse_map_reset(map);
	for (size_t i = 0; i < oldsize; i++)
	{
		if (old[i].x != INT_MAX)
		{
			size_t idx = sparse_map_index(map, old[i].x, old[i].y);
			map->table[idx] = old[i];
		}
	}
	free(old);
	return 0;
}

static void add_vortex(struct sparse_map *map, int x, int y)
{
	if (map->count * 100 >= map->size * 75
	    && sparse_map_rehash(map) < 0)
	{
		abort();
	}

	size_t idx = sparse_map_index(map, x, y);
	if (map->table[idx].x == INT_MAX)
	{
		map->count++;
		map->table[idx].x = x;
		map->table[idx].y = y;
		map->table[idx].count = 0;
	}
	map->table[idx].count++;
}

static void sparse_map_draw(struct sparse_map *map, int x0, int y0, int x1, int y1)
{
	int sx = x0 < x1 ? 1 : x0 > x1 ? -1 : 0;
	int sy = y0 < y1 ? 1 : y0 > y1 ? -1 : 0;
	while (1)
	{
		add_vortex(map, x0, y0);
		if (x0 == x1 && y0 == y1)
		{
			break;
		}
		x0 += sx;
		y0 += sy;
	}
}

static size_t sparse_map_count(struct sparse_map *map)
{
	size_t count = 0;
	for (size_t i = 0; i < map->size; i++)
	{
		if (map->table[i].x != INT_MAX
		    && map->table[i].count > 1)
		{
			count++;
		}
	}
	return count;
}

static size_t part1(struct sparse_map *map, struct segment *lst)
{
	sparse_map_reset(map);
	while (lst)
	{
		if (lst->x0 == lst->x1 || lst->y0 == lst->y1)
		{
			sparse_map_draw(map, lst->x0, lst->y0, lst->x1, lst->y1);
		}
		lst = lst->next;
	}
	return sparse_map_count(map);
}

static size_t part2(struct sparse_map *map, struct segment *lst)
{
	sparse_map_reset(map);
	while (lst)
	{
		sparse_map_draw(map, lst->x0, lst->y0, lst->x1, lst->y1);
		lst = lst->next;
	}
	return sparse_map_count(map);
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		exit(1);
	}

	FILE *input = fopen(argv[1], "rb");
	if (!input)
	{
		fprintf(stderr, "Cannot open %s\n", argv[1]);
		exit(1);
	}

	struct segment *head = NULL;
	int x0, y0, x1, y1;
	while (fscanf(input, " %d,%d -> %d,%d", &x0, &y0, &x1, &y1) == 4)
	{
		struct segment *s = malloc(sizeof(*s));
		if (!s)
		{
			break;
		}
		s->x0 = x0;
		s->y0 = y0;
		s->x1 = x1;
		s->y1 = y1;
		s->next = head;
		head = s;
	}
	fclose(input);

	struct sparse_map map = {0};
	printf("Part1: %zu\n", part1(&map, head));
	printf("Part2: %zu\n", part2(&map, head));

	free(map.table);
	while (head)
	{
		struct segment *next = head->next;
		free(head);
		head = next;
	}
	return 0;
}
