#include <algorithm>
#include <fstream>
#include <vector>
#include <fmt/format.h>

using namespace std;

struct Result
{
	uint64_t w1;
	uint64_t w2;
};

static Result cache[10][10][21][21];

static uint64_t part1(int p1, int p2)
{
	int s1 = 0;
	int s2 = 0;
	int dice = 1;
	uint64_t rolls = 0;
	for (;;)
	{
		int value = 0;
		for (int i = 0; i < 3; i++)
		{
			value += dice;
			if (++dice > 100)
			{
				dice = 1;
			}
		}
		rolls += 3;

		p1 = (p1 + value - 1) % 10 + 1;
		s1 += p1;
		if (s1 >= 1000)
		{
			break;
		}
		swap(p1, p2);
		swap(s1, s2);
	}
	return rolls * s2;
}

static Result winners(int p1, int p2, int s1, int s2)
{
	Result& r = cache[p1-1][p2-1][s1][s2];
	if (r.w1 != UINT64_MAX)
	{
		return r;
	}

	uint64_t w1 = 0;
	uint64_t w2 = 0;
	for (int i = 1; i <= 3; i++)
	{
		for (int j = 1; j <= 3; j++)
		{
			for (int k = 1; k <= 3; k++)
			{
				int np1 = (p1 + i + j + k - 1) % 10 + 1;
				int ns1 = s1 + np1;
				if (ns1 >= 21)
				{
					w1++;
				}
				else
				{
					Result s = winners(p2, np1, s2, ns1);
					w1 += s.w2;
					w2 += s.w1;
				}
			}
		}
	}
	r.w1 = w1;
	r.w2 = w2;
	return r;
}

static uint64_t part2(int p1, int p2)
{
	memset(cache, -1, sizeof(cache));
	auto r = winners(p1, p2, 0, 0);
	return r.w1 > r.w2 ? r.w1 : r.w2;
}

static istream& load_player(istream& in, int& p)
{
	string line;
	if (getline(in, line))
	{
		p = stoi(line.substr(line.rfind(" ")));
	}
	return in;
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

	int p1 = 0, p2 = 0;
	load_player(in, p1);
	load_player(in, p2);
	in.close();

	if (0 < p1 && p1 <= 10 && 0 < p2 && p2 <= 10)
	{
		fmt::print("Part1: {}\n", part1(p1, p2));
		fmt::print("Part2: {}\n", part2(p1, p2));
		return 0;
	}
	else
	{
		fmt::print("Not in the accepted range: {} {}\n", p1, p2);
		return 1;
	}
}
