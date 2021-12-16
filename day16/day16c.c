#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct bitstream
{
	const char *hexstr;
	uint64_t rawdata;
	int available;
	size_t pos;
};

static void bitstream_init(struct bitstream *bs, const char *hexstr)
{
	bs->hexstr = hexstr;
	bs->rawdata = bs->available = bs->pos = 0;
}

static unsigned bitstream_get(struct bitstream *bs, int needed)
{
	while (bs->available < needed)
	{
		if (*bs->hexstr == 0)
		{
			fprintf(stderr, "EOF\n");
			exit(1);
		}
		int val = *bs->hexstr++;
		if (isdigit(val))
		{
			val -= '0';
		}
		else if ('A' <= val && val <= 'F')
		{
			val -= 'A' - 10;
		}
		else
		{
			continue;
		}
		bs->rawdata = bs->rawdata << 4 | val;
		bs->available += 4;
	}
	bs->available -= needed;
	bs->pos += needed;

	unsigned value = bs->rawdata >> bs->available;
	bs->rawdata &= (1U << bs->available) - 1;
	return value;
}

struct node
{
	unsigned version;

	int type;
	union
	{
		uint64_t value;
		struct node *children;
	};

	struct node *next;
};

static struct node *program_parse(struct bitstream *bs)
{
	struct node *n = malloc(sizeof(*n));
	if (n)
	{
		n->version = bitstream_get(bs, 3);
		n->type = bitstream_get(bs, 3);
		n->next = NULL;
		if (n->type == 4)
		{
			n->value = 0;
			unsigned group;
			do
			{
				group = bitstream_get(bs, 5);
				n->value = n->value << 4 | (group & 0xF);
			} while (group & 0x10);
		}
		else if (bitstream_get(bs, 1))
		{
			struct node *head = NULL;
			struct node **tail = &head;

			unsigned count = bitstream_get(bs, 11);
			while (count-->0)
			{
				*tail = program_parse(bs);
				tail = &(*tail)->next;
			}

			n->children = head;
		}
		else
		{
			struct node *head = NULL;
			struct node **tail = &head;

			unsigned end = bitstream_get(bs, 15);
			end += bs->pos;
			while (bs->pos < end)
			{
				*tail = program_parse(bs);
				tail = &(*tail)->next;
			}

			n->children = head;
		}
	}
	return n;
}

static void program_free(struct node *n)
{
	if (n)
	{
		if (n->type != 4)
		{
			struct node *c = n->children;
			while (c)
			{
				struct node *next = c->next;
				program_free(c);
				c = next;
			}
		}
		free(n);
	}
}


static uint64_t interp1(struct node *n)
{
	if (n->type == 4)
	{
		return n->version;
	}

	uint64_t sum = n->version;
	for (n = n->children; n; n = n->next)
	{
		sum += interp1(n);
	}
	return sum;
}

static uint64_t interp2(struct node *n)
{
	uint64_t value;
	switch (n->type)
	{
	case 0:
		value = 0;
		for (n = n->children; n; n = n->next)
		{
			value += interp2(n);
		}
		break;

	case 1:
		value = 1;
		for (n = n->children; n; n = n->next)
		{
			value *= interp2(n);
		}
		break;

	case 2:
		value = UINT64_MAX;
		for (n = n->children; n; n = n->next)
		{
			uint64_t tmp = interp2(n);
			if (value > tmp)
			{
				value = tmp;
			}
		}
		break;

	case 3:
		value = 0;
		for (n = n->children; n; n = n->next)
		{
			uint64_t tmp = interp2(n);
			if (value < tmp)
			{
				value = tmp;
			}
		}
		break;

	case 4:
		value = n->value;
		break;

	case 5:
		value = interp2(n->children) > interp2(n->children->next);
		break;

	case 6:
		value = interp2(n->children) < interp2(n->children->next);
		break;

	case 7:
		value = interp2(n->children) == interp2(n->children->next);
		break;

	default:
		fprintf(stderr, "Unknown opcode\n");
		exit(1);
	}
	return value;
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

	char *line = 0;
	uint64_t lsize = 0;
	while (getline(&line, &lsize, in) != -1)
	{
		struct bitstream bs;
		bitstream_init(&bs, line);

		struct node *program = program_parse(&bs);
		printf("Part1: %lu\n", interp1(program));
		printf("Part2: %lu\n", interp2(program));
		program_free(program);
	}
	free(line);

	fclose(in);
	return 0;
}
