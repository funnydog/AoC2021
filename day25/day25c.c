#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct map
{
	char *data[3];
	size_t width;
	size_t height;
	int cur;
};

static void map_release(struct map *m)
{
	for (int i = 0; i < 3; i++)
	{
		free(m->data[i]);
	}
}

static void map_load(FILE *in, struct map *m)
{
	m->width = m->height = 0;
	m->cur = 0;

	char *line = NULL;
	size_t lsize = 0;
	ssize_t len;

	char *data = NULL;
	size_t count = 0;
	size_t size = 0;
	while ((len = getline(&line, &lsize, in)) != -1)
	{
		while (isspace(line[len-1]))
		{
			len--;
		}
		if (count + len >= size)
		{
			size_t nsize = size ? size * 2 : 4096;
			char *ndata = realloc(data, nsize);
			if (!ndata)
			{
				break;
			}
			size = nsize;
			data = ndata;
		}
		memmove(data+count, line, len);
		count += len;
		if (!m->width)
		{
			m->width = len;
		}
		m->height++;
	}
	free(line);

	data[count] = 0;
	m->data[0] = data;
	m->data[1] = strdup(data);
	m->data[2] = strdup(data);
}

static void map_single_step(struct map *m, int herd, unsigned dx, unsigned dy)
{
	char *src = m->data[m->cur];
	if (++m->cur == 3)
	{
		m->cur = 0;
	}
	char *dst = m->data[m->cur];

	memset(dst, '.', m->width * m->height);
	for (size_t y = 0, off = 0; y < m->height; y++)
	{
		for (size_t x = 0; x < m->width; x++, off++)
		{
			if (src[off] == '.')
			{
			}
			else if (src[off] != herd)
			{
				dst[off] = src[off];
			}
			else
			{
				size_t nx = x;
				if ((nx += dx) == m->width)
				{
					nx = 0;
				}
				size_t ny = y;
				if ((ny += dy) == m->height)
				{
					ny = 0;
				}
				size_t noff = ny * m->width + nx;
				if (src[noff] == '.')
				{
					dst[noff] = herd;
				}
				else
				{
					dst[off] = herd;
				}
			}
		}
	}
}

static void map_step(struct map *m)
{
	map_single_step(m, '>', 1, 0);
	map_single_step(m, 'v', 0, 1);
}

static int map_deadlock(struct map *m)
{
	int a = m->cur;
	int b = (a + 1) % 3;
	return !strcmp(m->data[a], m->data[b]);
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

	struct map m;
	map_load(in, &m);
	fclose(in);

	int i = 0;
	do
	{
		map_step(&m);
		i++;
	} while (!map_deadlock(&m));
	map_release(&m);

	printf("Part1: %d\n", i);
	return 0;
}
