#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct vec3
{
	int v[3];
} vec3;

struct cuboid
{
	vec3 p[2];

	struct cuboid *next;
};

struct instr
{
	enum { ON, OFF } cmd;
	struct cuboid cuboid;

	struct instr *next;
};

static struct cuboid *freelist = NULL;

static void cuboid_free(struct cuboid *c)
{
	if (c)
	{
		c->next = freelist;
		freelist = c;
	}
}

static struct cuboid *cuboid_clone(const struct cuboid *c, struct cuboid *next)
{
	struct cuboid *n;
	if (freelist)
	{
		n = freelist;
		freelist = n->next;
	}
	else if (!(n = malloc(sizeof(*n))))
	{
		return next;
	}

	*n = *c;
	n->next = next;
	return n;
}

static int cuboid_inside(const struct cuboid *a, const struct cuboid *b)
{
	for (int i = 0; i < 3; i++)
	{
		if (a->p[0].v[i] < b->p[0].v[i] || a->p[1].v[i] > b->p[1].v[i])
		{
			return 0;
		}
	}
	return 1;
}

static int cuboid_intersect(const struct cuboid *a, const struct cuboid *b)
{
	for (int i = 0; i < 3; i++)
	{
		if (a->p[1].v[i] <= b->p[0].v[i] || a->p[0].v[i] >= b->p[1].v[i])
		{
			return 0;
		}
	}
	return 1;
}

static uint64_t cuboid_volume(const struct cuboid *c)
{
	uint64_t v = 1;
	for (int i = 0; i < 3; i++)
	{
		v *= c->p[1].v[i] - c->p[0].v[i];
	}
	return v;
}

static struct instr *parse_instructions(FILE *in)
{
	char *line = NULL;
	size_t lsize = 0;
	ssize_t len;
	struct instr *head = NULL;
	struct instr **tail = &head;
	struct instr tmp;
	while ((len = getline(&line, &lsize, in)) != -1)
	{
		char *tok = strtok(line, " ");
		if (!tok)
		{
			break;
		}
		else if (!strcmp(tok, "on"))
		{
			tmp.cmd = ON;
		}
		else if (!strcmp(tok, "off"))
		{
			tmp.cmd = OFF;
		}
		else
		{
			break;
		}

		tok = strtok(NULL, " \r\n");
		if (!tok || sscanf(tok, "x=%d..%d,y=%d..%d,z=%d..%d",
				   &tmp.cuboid.p[0].v[0],
				   &tmp.cuboid.p[1].v[0],
				   &tmp.cuboid.p[0].v[1],
				   &tmp.cuboid.p[1].v[1],
				   &tmp.cuboid.p[0].v[2],
				   &tmp.cuboid.p[1].v[2]) != 6)
		{
			break;
		}
		tmp.cuboid.p[1].v[0]++;
		tmp.cuboid.p[1].v[1]++;
		tmp.cuboid.p[1].v[2]++;

		struct instr *i = malloc(sizeof(*i));
		if (!i)
		{
			break;
		}
		*i = tmp;
		i->next = NULL;
		*tail = i;
		tail = &i->next;
	}
	free(line);
	return head;
}

static struct cuboid *split(const struct cuboid *a, const struct cuboid *b, struct cuboid *end)
{
	struct cuboid *res = cuboid_clone(a, end);
	struct cuboid *tmp = end;
	for (int j = 0; j < 2; j++)
	{
		for (int i = 0; i < 3; i++)
		{
			for (struct cuboid *c = res; c != end; )
			{
				struct cuboid *cnext = c->next;
				if (cuboid_intersect(c, b)
				    && c->p[0].v[i] <= b->p[j].v[i]
				    && b->p[j].v[i] < c->p[1].v[i])
				{
					struct cuboid t = *c;
					t.p[1].v[i] = b->p[j].v[i];
					if (!cuboid_inside(&t, b))
					{
						tmp = cuboid_clone(&t, tmp);
					}

					t = *c;
					t.p[0].v[i] = b->p[j].v[i];
					if (!cuboid_inside(&t, b))
					{
						tmp = cuboid_clone(&t, tmp);
					}
					cuboid_free(c);
				}
				else
				{
					c->next = tmp;
					tmp = c;
				}
				c = cnext;
			}
			res = tmp;
			tmp = end;
		}
	}

	return res;
}

static struct cuboid *sub(struct cuboid *lst, const struct cuboid *b)
{
	struct cuboid *res = NULL;

	for (struct cuboid *c = lst; c;)
	{
		struct cuboid *cnext = c->next;
		if (cuboid_inside(c, b))
		{
			cuboid_free(c);
		}
		else if (cuboid_intersect(c, b))
		{
			res = split(c, b, res);
			cuboid_free(c);
		}
		else
		{
			c->next = res;
			res = c;
		}
		c = cnext;
	}

	return res;
}

static struct cuboid *add(struct cuboid *lst, const struct cuboid *b)
{
	return cuboid_clone(b, sub(lst, b));
}

static uint64_t volume(const struct cuboid *lst)
{
	uint64_t v = 0;
	for (; lst; lst = lst->next)
	{
		v += cuboid_volume(lst);
	}
	return v;
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

	struct instr *prog = parse_instructions(in);
	fclose(in);

	uint64_t part1 = 0;
	struct cuboid region = {
		.p = {{{-50, -50, -50}},{{51, 51, 51}}},
	};

	/* list of non overlapping cuboids */
	struct cuboid *lst = NULL;
	for (struct instr *i = prog; i; i = i->next)
	{
		if (!part1 && !cuboid_inside(&i->cuboid, &region))
		{
			part1 = volume(lst);
		}
		switch (i->cmd)
		{
		case ON: lst = add(lst, &i->cuboid); break;
		case OFF: lst = sub(lst, &i->cuboid); break;
		}
	}
	printf("Part1: %lu\n", part1);
	printf("Part2: %lu\n", volume(lst));

	/* cleanup */
	while (lst)
	{
		struct cuboid *next = lst->next;
		cuboid_free(lst);
		lst = next;
	}

	while (freelist)
	{
		struct cuboid *next = freelist->next;
		free(freelist);
		freelist = next;
	}

	while (prog)
	{
		struct instr *next = prog->next;
		free(prog);
		prog = next;
	}

	return 0;
}
