#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum cmd { FORWARD, DOWN, UP };

struct instr
{
	enum cmd cmd;
	int arg;
};

static int part1(struct instr *arr, size_t count)
{
	int x = 0;
	int depth = 0;

	while (count-->0)
	{
		switch (arr->cmd)
		{
		case FORWARD: x += arr->arg; break;
		case DOWN:    depth += arr->arg; break;
		case UP:      depth -= arr->arg; break;
		}
		arr++;
	}

	return x * depth;
}

static int part2(struct instr *arr, size_t count)
{
	int x = 0;
	int depth = 0;
	int aim = 0;

	while (count-->0)
	{
		switch (arr->cmd)
		{
		case FORWARD:
			x += arr->arg;
			depth += aim * arr->arg;
			break;

		case DOWN:
			aim += arr->arg;
			break;

		case UP:
			aim -= arr->arg;
			break;
		}
		arr++;
	}

	return x * depth;
}

static struct instr *parse_file(FILE *in, size_t *count)
{
	struct instr *arr = NULL;
	size_t acount = 0;
	size_t asize = 0;
	size_t lsize = 0;
	char *line = NULL;
	while (getline(&line, &lsize, in))
	{
		enum cmd cmd;
		char *tok = strtok(line, " \t\r\n");
		if (!tok)
		{
			break;
		}
		if (!strcmp(tok, "forward"))
		{
			cmd = FORWARD;
		}
		else if (!strcmp(tok, "down"))
		{
			cmd = DOWN;
		}
		else if (!strcmp(tok, "up"))
		{
			cmd = UP;
		}
		else
		{
			break;
		}

		if (!(tok = strtok(NULL, " \t\r\n")))
		{
			break;
		}

		if (acount >= asize)
		{
			size_t nsize = asize ? (asize * 2) : 128;
			struct instr *narr = realloc(arr, nsize * sizeof(*narr));
			if (!narr)
			{
				break;
			}
			asize = nsize;
			arr = narr;
		}
		arr[acount].cmd = cmd;
		arr[acount].arg = strtol(tok, NULL, 10);
		acount++;
	}
	free(line);

	*count = acount;
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
		return 1;
	}
	size_t count;
	struct instr *prog = parse_file(in, &count);
	fclose(in);

	printf("Part1: %d\n", part1(prog, count));
	printf("Part2: %d\n", part2(prog, count));

	free(prog);

	return 0;
}
