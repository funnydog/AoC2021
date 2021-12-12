#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct edge
{
	const char *node;
	struct edge *next;
};

struct node
{
	char *key;
	struct edge *edges;
};

struct graph
{
	struct node *nodes;
	size_t count;
	size_t size;
};

static size_t hashfn(const char *str)
{
	size_t hash = 5381;
	for(; *str; str++)
	{
		hash = hash * 33 + *str;
	}
	return hash;
}

static int rehash(struct graph *g)
{
	size_t size = g->size ? g->size * 2 : 8;
	struct node *nodes = calloc(size, sizeof(*nodes));
	if (!nodes)
	{
		return -1;
	}
	for (size_t i = 0; i < g->size; i++)
	{
		if (!g->nodes[i].key)
		{
			continue;
		}

		size_t idx = hashfn(g->nodes[i].key) % size;
		while (nodes[idx].key)
		{
			if (++idx >= size)
			{
				idx = 0;
			}
		}
		nodes[idx] = g->nodes[i];
	}
	free(g->nodes);
	g->nodes = nodes;
	g->size = size;
	return 0;
}

static size_t find_node(const struct graph *g, const char *key)
{
	size_t idx = hashfn(key) % g->size;
	while (g->nodes[idx].key && strcmp(g->nodes[idx].key, key))
	{
		if (++idx >= g->size)
		{
			idx = 0;
		}
	}
	return idx;
}

static void add_node(struct graph *g, const char *str)
{
	if (g->count * 100 >= g->size * 75 && rehash(g) < 0)
	{
		abort();
	}
	size_t idx = find_node(g, str);
	if  (!g->nodes[idx].key)
	{
		g->nodes[idx].key = strdup(str);
		g->nodes[idx].edges = NULL;
		g->count++;
	}
}

static void add_edge(struct graph *g, const char *src, const char *dst)
{
	struct edge *e = malloc(sizeof(*e));
	if (e)
	{
		size_t idx = find_node(g, dst);
		e->node = g->nodes[idx].key;
		idx = find_node(g, src);
		e->next = g->nodes[idx].edges;
		g->nodes[idx].edges = e;
	}
}

static void graph_release(struct graph *g)
{
	for (size_t i = 0; i < g->size; i++)
	{
		if (g->nodes[i].key)
		{
			struct edge *e = g->nodes[i].edges;
			while (e)
			{
				struct edge *next = e->next;
				free(e);
				e = next;
			}
			free(g->nodes[i].key);
		}
	}
	free(g->nodes);
}

static void graph_load(FILE *in, struct graph *g)
{
	char *line = NULL;
	size_t lsize = 0;
	while (getline(&line, &lsize, in))
	{
		char *src = strtok(line, "-\r\n");
		if (!src)
		{
			break;
		}
		char *dst = strtok(NULL, "\r\n");
		if (!dst)
		{
			break;
		}
		add_node(g, src);
		add_node(g, dst);
		add_edge(g, src, dst);
		add_edge(g, dst, src);
	}
	free(line);
}

struct array
{
	size_t *data;
	size_t count;
	size_t size;
};

static void array_release(struct array *a)
{
	free(a->data);
}

static void array_append(struct array *a, size_t i)
{
	if (a->count == a->size)
	{
		size_t size = a->size ? a->size * 2 : 128;
		size_t *data = realloc(a->data, size * sizeof(*data));
		if (!data)
		{
			abort();
		}
		a->size = size;
		a->data = data;
	}
	a->data[a->count++] = i;
}

static size_t backtrack(struct array *arr, bool assigned, const struct graph *g)
{
	size_t cur = arr->data[arr->count - 1];
	if (!strcmp(g->nodes[cur].key, "end"))
	{
		return 1;
	}

	size_t count = 0;
	for (struct edge *e = g->nodes[cur].edges;
	     e;
	     e = e->next)
	{
		size_t idx = find_node(g, e->node);
		bool should_skip = false;
		if (e->node[0] != toupper(e->node[0]))
		{
			for (size_t i = 1; i < arr->count; i++)
			{
				if (arr->data[i] == idx)
				{
					should_skip = true;
					break;
				}
			}
		}

		if ((assigned && should_skip) || idx == arr->data[0])
		{
			continue;
		}

		array_append(arr, idx);
		count += backtrack(arr, assigned || should_skip, g);
		arr->count--;
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

	struct array a = {0};
	struct graph g = {0};
	graph_load(in, &g);
	fclose(in);

	array_append(&a, find_node(&g, "start"));
	printf("Part1: %zu\n", backtrack(&a, true, &g));
	printf("Part2: %zu\n", backtrack(&a, false, &g));

	graph_release(&g);
	array_release(&a);

	return 0;
}
