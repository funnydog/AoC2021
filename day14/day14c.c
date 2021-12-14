#include <assert.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct rule
{
	char key[2];
	int insert;
	size_t count;
	size_t swap;
};

struct ruleset
{
	struct rule *rules;
	size_t count;
	size_t size;

	size_t letters[256];
};

static unsigned hashfn(const char *str)
{
	return (5381 * 33 + str[0]) * 33 + str[1];
}

static int rehash(struct ruleset *r)
{
	size_t size = r->size ? r->size * 2 : 32;
	struct rule *rules = calloc(size, sizeof(*rules));
	if (!rules)
	{
		return -1;
	}

	for (size_t i = 0; i < r->size; i++)
	{
		if (r->rules[i].key[0])
		{
			size_t idx = hashfn(r->rules[i].key) % size;
			while (rules[idx].key[0])
			{
				if (++idx == size)
				{
					idx = 0;
				}
			}
			rules[idx] = r->rules[i];
		}
	}
	free(r->rules);
	r->rules = rules;
	r->size = size;
	return 0;
}

static struct rule *get_rule(struct ruleset *rs, int a, int b)
{
	if (rs->count * 100 >= rs->size * 75 && rehash(rs) < 0)
	{
		fprintf(stderr, "Cannot rehash the ruleset\n");
		abort();
	}
	char pattern[] = {a, b};
	size_t idx = hashfn(pattern) % rs->size;
	struct rule *r = rs->rules + idx;

	while (r->key[0] && memcmp(r->key, pattern, 2))
	{
		if (++idx == rs->size)
		{
			idx = 0;
		}
		r = rs->rules + idx;
	}

	if (!r->key[0])
	{
		r->key[0] = a;
		r->key[1] = b;
		r->insert = -1;
		r->count = 0;
		rs->count++;
	}

	return r;
}

static void ruleset_release(struct ruleset *r)
{
	free(r->rules);
}

static void ruleset_load(FILE *in, struct ruleset *rs)
{
	char *line = NULL;
	size_t sline = 0;
	ssize_t len;
	char *template = NULL;

	enum { TEMPLATE, RULES } state = TEMPLATE;
	while ((len = getline(&line, &sline, in)) != -1)
	{
		/* trim the line */
		while (len && isspace(line[len-1]))
		{
			line[--len] = 0;
		}

		/* load the value */
		if (state == TEMPLATE)
		{
			if (len == 0)
			{
				state = RULES;
			}
			else
			{
				template = line;
				line = NULL;
				sline = 0;
			}
		}
		else if (len)
		{
			struct rule *r = get_rule(rs, line[0], line[1]);
			r->insert = line[len-1];
		}
	}
	free(line);

	/* populate the count of the couples and the letter count */
	if (template)
	{
		memset(rs->letters, 0, sizeof(rs->letters));
		for (line = template; *line; line++)
		{
			if (line[1])
			{
				struct rule *r = get_rule(rs, line[0], line[1]);
				if (r)
				{
					r->count++;
				}
			}
			rs->letters[(unsigned)line[0]]++;
		}
	}
	free(template);
}

static size_t solve(struct ruleset *rs, size_t steps)
{
	while (steps-->0)
	{
		/* copy the actual count into the temporary */
		for (size_t i = 0; i < rs->size; i++)
		{
			rs->rules[i].swap = rs->rules[i].count;
			rs->rules[i].count = 0;
		}

		/* for each present couple apply the rules */
		for (size_t i = 0; i < rs->size; i++)
		{
			struct rule *r = rs->rules + i;
			if (!r->swap)
			{
				/* couple not actually present */
			}
			else if (r->insert == -1)
			{
				/* this couple doesn't transform itself */
				r->count += r->swap;
			}
			else
			{
				/* transform every "AB -> C" to AC CB */
				struct rule *d = get_rule(rs, r->key[0], r->insert);
				if (d)
				{
					d->count += r->swap;
				}

				d = get_rule(rs, r->insert, r->key[1]);
				if (d)
				{
					d->count += r->swap;
				}

				/* count the new inserted letters */
				rs->letters[r->insert] += r->swap;
			}
		}
	}

	/* find the maximum and minimum letters */
	size_t max = 0;
	size_t min = SIZE_MAX;
	for (size_t i = 0; i < 256; i++)
	{
		size_t c = rs->letters[i];
		if (c == 0)
		{
			continue;
		}
		if (min > c)
		{
			min = c;
		}
		if (max < c)
		{
			max  = c;
		}
	}

	return (max > min) ? max - min : 0;
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
	struct ruleset r = {0};
	ruleset_load(in, &r);
	fclose(in);

	printf("Part1: %zu\n", solve(&r, 10));
	printf("Part2: %zu\n", solve(&r, 30));

	ruleset_release(&r);
	return 0;
}
