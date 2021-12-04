#include <fstream>
#include <sstream>
#include <vector>
#include <fmt/format.h>

using namespace std;

class Board
{
public:
	Board()
	{
		for (size_t i = 0; i < 25; i++)
		{
			numbers[i] = 0;
			marked[i] = false;
		}
		won = false;
	}

	bool mark_and_check(int num)
	{
		size_t pos;
		for (pos = 0; pos < 25; pos++)
		{
			if (numbers[pos] == num)
			{
				break;
			}
		}
		if (pos < 25)
		{
			marked[pos] = true;
			won = won || check_row(pos) || check_col(pos);
		}
		return won;
	}

	int score(int num) const
	{
		int score = 0;
		for (size_t i = 0; i < 25; i++)
		{
			if (!marked[i])
			{
				score += numbers[i];
			}
		}
		return num * score;
	}

private:
	bool check_row(size_t pos) const
	{
		pos -= pos % 5;
		for (size_t end = pos + 5; pos < end; pos++)
		{
			if (!marked[pos])
			{
				return false;
			}
		}
		return true;
	}

	bool check_col(size_t pos) const
	{
		for (pos %= 5; pos < 25; pos += 5)
		{
			if (!marked[pos])
			{
				return false;
			}
		}
		return true;
	}

	int numbers[25];
	bool marked[25];
public:
	bool won;

	friend istream& operator>>(istream&, Board& b);
};

istream& operator>>(istream& in, Board& b)
{
	for (size_t i = 0; i < 25; i++)
	{
		if (!(in >> b.numbers[i]))
		{
			break;
		}
	}
	return in;
}

istream& operator>>(istream& in, vector<int>& numbers)
{
	string line;
	if (!getline(in, line))
	{
		return in;
	}

	istringstream is(line);
	string num;
	while (getline(is, num, ','))
	{
		numbers.push_back(stoi(num));
	}

	return in;
}

static int part1(vector<int>& numbers, vector<Board>& boards)
{
	for (auto n = numbers.begin();
	     n != numbers.end();
	     n = numbers.erase(n))
	{
		for (auto& b: boards)
		{
			if (b.mark_and_check(*n))
			{
				return b.score(*n);
			}
		}
	}
	return 0;
}

static int part2(vector<int>& numbers, vector<Board>& boards)
{
	for (auto n: numbers)
	{
		for (auto i = boards.begin(); i != boards.end(); )
		{
			if (!i->mark_and_check(n))
			{
				++i;
			}
			else if (boards.size() != 1)
			{
				i = boards.erase(i);
			}
			else
			{
				return i->score(n);
			}
		}
	}
	return 0;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fmt::print(stderr, "Usage: {} <filename>\n", argv[0]);
		return 1;
	}

	ifstream in(argv[1]);
	if (!in)
	{
		fmt::print(stderr, "Cannot open {}\n", argv[1]);
		return 1;
	}

	vector<int> numbers;
	in >> numbers;

	vector<Board> boards;
	Board b;
	while (in >> b)
	{
		boards.emplace_back(b);
	}
	in.close();

	fmt::print("Part1: {}\n", part1(numbers, boards));
	fmt::print("Part2: {}\n", part2(numbers, boards));
	return 0;
}
