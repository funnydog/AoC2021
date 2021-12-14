#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct board
{
	int numbers[25];
	int marked[25];
	int won;

	struct board *next;
};

static int check_row(struct board *b, int pos)
{
	pos -= pos % 5;
	for (int end = pos+5; pos < end; pos++)
	{
		if (!b->marked[pos])
		{
			return 0;
		}
	}
	return 1;
}

static int check_col(struct board *b, int pos)
{
	for (pos %= 5; pos < 25; pos += 5)
	{
		if (!b->marked[pos])
		{
			return 0;
		}
	}
	return 1;
}

static int mark_and_check(struct board *b, int number)
{
	unsigned pos;
	for (pos = 0; pos < 25; pos++)
	{
		if (b->numbers[pos] == number)
		{
			break;
		}
	}
	if (pos < 25)
	{
		b->marked[pos] = 1;
		b->won = b->won || check_row(b, pos) || check_col(b, pos);
	}
	return b->won;
}

static int score(struct board *b, int number)
{
	int score = 0;
	for (size_t i = 0; i < 25; i++)
	{
		if (!b->marked[i])
		{
			score += b->numbers[i];
		}
	}
	return score * number;
}

static int *load_numbers(FILE *in, size_t *count)
{
	size_t lsize = 0;
	char *line = NULL;
	int *numbers = NULL;
	size_t ncount = 0;
	if (getline(&line, &lsize, in) != -1)
	{
		size_t size = 0;
		char *tok = strtok(line, ",\n");
		while (tok)
		{
			if (ncount == size)
			{
				size_t nsize = size ? size * 2 : 128;
				int *nnums = realloc(numbers, nsize * sizeof(*nnums));
				if (!nnums)
				{
					break;
				}
				size = nsize;
				numbers = nnums;
			}
			numbers[ncount++] = strtol(tok, NULL, 10);
			tok = strtok(NULL, ",\n");
		}
	}
	free(line);
	*count = ncount;
	return numbers;
}

static struct board *load_boards(FILE *in)
{
	struct board *head = NULL;
	while (1)
	{
		int numbers[25];
		for (size_t i = 0; i < 25; i++)
		{
			if (fscanf(in, " %d", &numbers[i]) != 1)
			{
				goto out;
			}
		}
		struct board *board = calloc(1, sizeof(*board));
		if (!board)
		{
			break;
		}
		memmove(board->numbers, numbers, sizeof(board->numbers));
		board->next = head;
		head = board;
	}
out:
	return head;
}

static int part1(struct board *lst, int *numbers, size_t ncount)
{
	for (size_t i = 0; i < ncount; i++)
	{
		for (struct board *b = lst; b; b = b->next)
		{
			if (mark_and_check(b, numbers[i]))
			{
				return score(b, numbers[i]);
			}
		}
	}
	return 0;
}

static int part2(struct board **lst, int *numbers, size_t ncount)
{
	for (size_t i = 0; i < ncount; i++)
	{
		struct board **b = lst;
		while (*b)
		{
			if (!mark_and_check(*b, numbers[i]))
			{
				b = &(*b)->next;
			}
			else if ((*lst)->next != NULL)
			{
				struct board *tmp = *b;
				*b = tmp->next;
				free(tmp);
			}
			else
			{
				return score(*b, numbers[i]);
			}
		}
	}
	return 0;
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
	int *numbers = load_numbers(in, &count);
	struct board *boards = load_boards(in);
	fclose(in);

	printf("Part1: %d\n", part1(boards, numbers, count));
	printf("Part2: %d\n", part2(&boards, numbers, count));

	while (boards)
	{
		struct board *next = boards->next;
		free(boards);
		boards = next;
	}
	free(numbers);
	return 0;
}
