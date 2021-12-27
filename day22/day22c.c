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
	vec3 p0, p1;

	struct cuboid *next;
};

struct instr
{
	enum { ON, OFF } cmd;
	struct cuboid cuboid;

	struct instr *next;
};

static struct cuboid *cuboid_clone(const struct cuboid *c, struct cuboid *next)
{
	struct cuboid *n = malloc(sizeof(*n));
	if (!n)
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
		if (a->p0.v[i] < b->p0.v[i] || a->p1.v[i] > b->p1.v[i])
		{
			return 0;
		}
	}
	return 1;
}

static struct cuboid *cuboid_intersect(const struct cuboid *a, const struct cuboid *b, struct cuboid *end)
{
	struct cuboid tmp;
	for (int i = 0; i < 3; i++)
	{
		int v0 = a->p0.v[i] > b->p0.v[i] ? a->p0.v[i] : b->p0.v[i];
		int v1 = a->p1.v[i] < b->p1.v[i] ? a->p1.v[i] : b->p1.v[i];
		if (v0 >= v1)
		{
			return end;
		}
		tmp.p0.v[i] = v0;
		tmp.p1.v[i] = v1;
	}
	return cuboid_clone(&tmp, end);
}

static uint64_t cuboid_volume(const struct cuboid *c)
{
	uint64_t v = 1;
	for (int i = 0; i < 3; i++)
	{
		v *= c->p1.v[i] - c->p0.v[i];
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
				   &tmp.cuboid.p0.v[0],
				   &tmp.cuboid.p1.v[0],
				   &tmp.cuboid.p0.v[1],
				   &tmp.cuboid.p1.v[1],
				   &tmp.cuboid.p0.v[2],
				   &tmp.cuboid.p1.v[2]) != 6)
		{
			break;
		}
		tmp.cuboid.p1.v[0]++;
		tmp.cuboid.p1.v[1]++;
		tmp.cuboid.p1.v[2]++;

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

static void sub(struct cuboid **on, struct cuboid **off, const struct cuboid *b)
{
	struct cuboid *oldon = *on;
	for (struct cuboid *c = *off; c; c = c->next)
	{
		*on = cuboid_intersect(c, b, *on);
	}

	for (struct cuboid *c = oldon; c; c = c->next)
	{
		*off = cuboid_intersect(c, b, *off);
	}
}

static void add(struct cuboid **on, struct cuboid **off, const struct cuboid *b)
{
	sub(on, off, b);
	*on = cuboid_clone(b, *on);
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

static void cuboid_free(struct cuboid *c)
{
	while (c)
	{
		struct cuboid *next = c->next;
		free(c);
		c = next;
	}
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
		.p0 = {{-50, -50, -50}},
		.p1 = {{ 51,  51,  51}},
	};

	/* list of non overlapping cuboids */
	struct cuboid *on = NULL;
	struct cuboid *off = NULL;
	for (struct instr *i = prog; i; i = i->next)
	{
		if (!part1 && !cuboid_inside(&i->cuboid, &region))
		{
			part1 = volume(on) - volume(off);
		}
		switch (i->cmd)
		{
		case ON: add(&on, &off, &i->cuboid); break;
		case OFF: sub(&on, &off, &i->cuboid); break;
		}
	}
	printf("Part1: %lu\n", part1);
	printf("Part2: %lu\n", volume(on) - volume(off));

	cuboid_free(on);
	cuboid_free(off);
	while (prog)
	{
		struct instr *next = prog->next;
		free(prog);
		prog = next;
	}

	return 0;
}
