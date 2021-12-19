#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct number
{
	struct number *up;
	enum { PAIR, NUMBER } type;
	int value;
	struct number *left;
	struct number *right;
};

static struct number *freelist = NULL;

static struct number *make_number(struct number *parent, int value)
{
	struct number *n;
	if (freelist)
	{
		n = freelist;
		freelist = n->right;
	}
	else
	{
		n = malloc(sizeof(*n));
	}
	if (n)
	{
		n->up = parent;
		n->type = NUMBER;
		n->value = value;
		n->left = n->right = NULL;
	}
	return n;
}

static struct number *make_pair(struct number *parent)
{
	struct number *n = make_number(parent, 0);
	if (n)
	{
		n->type = PAIR;
	}
	return n;
}

static void free_number(struct number *n)
{
	if (!n)
	{
		return;
	}

	if (n->type == PAIR)
	{
		free_number(n->left);
		free_number(n->right);
	}
	n->right = freelist;
	freelist = n;
}

enum { LPAR, RPAR, COMMA, NUM, EOT };
static int scan_value;
static char scan_char;

static int scan_get(FILE *in)
{
	while (scan_char != EOF && (!scan_char || isspace(scan_char)))
	{
		scan_char = getc(in);
	}
	switch (scan_char)
	{
	case EOF: return EOT;
	case '[': scan_char = getc(in); return LPAR;
	case ']': scan_char = getc(in); return RPAR;
	case ',': scan_char = getc(in); return COMMA;
	default:
		if (isdigit(scan_char))
		{
			scan_value = 0;
			while (isdigit(scan_char))
			{
				scan_value = scan_value * 10 + scan_char - '0';
				scan_char = getc(in);
			}
			return NUM;
		}
		return EOT;
	}
}

static int sym;

static struct number *parse_number(FILE *in, struct number *parent)
{
	if (sym == LPAR)
	{
		struct number *n = make_pair(parent);
		sym = scan_get(in);
		n->left = parse_number(in, n);
		sym = scan_get(in);
		n->right = parse_number(in, n);
		sym = scan_get(in);
		return n;
	}
	else
	{
		struct number *n = make_number(parent, scan_value);
		sym = scan_get(in);
		return n;
	}
}

static struct number *clone_number(const struct number *n, struct number *parent)
{
	if (n->type == NUMBER)
	{
		return make_number(parent, n->value);
	}
	else
	{
		struct number *r = make_pair(parent);
		if (r)
		{
			r->left = clone_number(n->left, r);
			r->right = clone_number(n->right, r);
		}
		return r;
	}
}

static void add_left(struct number *n, int value)
{
	while (n->up && n->up->left == n)
	{
		n = n->up;
	}
	if (n->up)
	{
		n = n->up->left;
		while (n && n->type == PAIR)
		{
			n = n->right;
		}
		if (n)
		{
			n->value += value;
		}
	}
}

static void add_right(struct number *n, int value)
{
	while (n->up && n->up->right == n)
	{
		n = n->up;
	}
	if (n->up)
	{
		n = n->up->right;
		while (n && n->type == PAIR)
		{
			n = n->left;
		}
		if (n)
		{
			n->value += value;
		}
	}
}

static void explode(struct number *n, int lvl)
{
	if (lvl >= 4 && n->type == PAIR)
	{
		add_left(n, n->left->value);
		add_right(n, n->right->value);
		free_number(n->left);
		free_number(n->right);
		n->left = n->right = NULL;
		n->type = NUMBER;
		n->value = 0;
	}
	else if (n->type == PAIR)
	{
		explode(n->left, lvl+1);
		explode(n->right, lvl+1);
	}
	else
	{
		/* nothing */
	}
}

static int action;
static void split(struct number *n, int lvl)
{
	if (action)
	{
		return;
	}
	else if (n->type == PAIR)
	{
		split(n->left, lvl+1);
		split(n->right, lvl+1);
	}
	else if (n->value >= 10)
	{
		action = 1;
		n->type = PAIR;
		n->left = make_number(n, n->value / 2);
		n->right = make_number(n, (n->value+1) / 2);
		explode(n, lvl);
	}
	else
	{
		/* nothing */
	}
}

static void reduce(struct number *n)
{
	explode(n, 0);
	do
	{
		action = 0;
		split(n, 0);
	} while (action);
}

static struct number *add(const struct number *a, const struct number *b)
{
	struct number *n = make_pair(NULL);
	if (n)
	{
		n->left = clone_number(a, n);
		n->right = clone_number(b, n);
		reduce(n);
	}
	return n;
}

static int magnitude(const struct number *n)
{
	if (n->type == NUMBER)
	{
		return n->value;
	}
	else
	{
		return 3 * magnitude(n->left)
			+ 2 * magnitude(n->right);
	}
}

static void print_number(struct number *n)
{
	if (n->type == NUMBER)
	{
		printf("%d", n->value);
	}
	else
	{
		putc('[', stdout);
		print_number(n->left);
		putc(',', stdout);
		print_number(n->right);
		putc(']', stdout);
	}
}

static struct number **load(FILE *in, size_t *cnt)
{
	struct number **arr = NULL;
	size_t count = 0;
	size_t size = 0;

	sym = scan_get(in);
	while (!feof(in))
	{
		if (count == size)
		{
			size_t nsize = size ? size * 2 : 8;
			struct number **narr = realloc(arr, nsize * sizeof(*narr));
			if (!narr)
			{
				break;
			}
			size = nsize;
			arr = narr;
		}
		struct number *n = parse_number(in, NULL);
		if (n)
		{
			arr[count++] = n;
		}
	}
	*cnt = count;
	return arr;
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

	(void)clone_number;
	size_t count;
	struct number **arr = load(in, &count);
	fclose(in);

	struct number *res = clone_number(arr[0], NULL);
	for (size_t i = 1; i < count; i++)
	{
		struct number *n = add(res, arr[i]);
		free_number(res);
		res = n;
	}
	printf("Part1: %d\n", magnitude(res));
	free_number(res);

	int max = 0;
	for (size_t i = 0; i < count; i++)
	{
		for (size_t j = 0; j < count; j++)
		{
			if (i == j)
			{
				continue;
			}
			struct number *n = add(arr[i], arr[j]);
			int m = magnitude(n);
			free_number(n);
			if (max < m)
			{
				max = m;
			}
		}
	}
	printf("Part2: %d\n", max);

	for (size_t i = 0; i < count; i++)
	{
		free_number(arr[i]);
	}
	while (freelist)
	{
		struct number *next = freelist->right;
		free(freelist);
		freelist = next;
	}
	free(arr);
	(void)print_number;
	return 0;
}
