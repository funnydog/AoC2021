#include <algorithm>
#include <array>
#include <fstream>
#include <unordered_map>
#include <fmt/format.h>

using namespace std;

struct Rule
{
	int insert;
	size_t count;
	size_t other;
};

struct HashRule
{
	size_t operator()(const pair<int,int>& k) const
	{
		return (5381 * 33 + k.first) * 33 + k.second;
	}
};

using Ruleset = unordered_map<pair<int,int>,Rule,HashRule>;
using Letterset = unordered_map<int,size_t>;

static istream& parse_problem(istream& in, Ruleset &r, Letterset &letters)
{
	r.clear();
	letters.clear();

	enum { TEMPLATE, RULE } state = TEMPLATE;
	string line;
	string templ;
	while (getline(in, line))
	{
		if (state == TEMPLATE)
		{
			if (line.empty())
			{
				state = RULE;
			}
			else
			{
				templ = line;
			}
		}
		else if (!line.empty())
		{
			r[make_pair(line[0], line[1])].insert = line.back();
		}
		else
		{
			break;
		}
	}

	for (auto it = templ.begin(); it != templ.end(); ++it)
	{
		letters[(unsigned)*it]++;
		auto next = it + 1;
		if (next != templ.end())
		{
			r[make_pair(*it, *next)].count++;
		}
	}

	return in;
}

static size_t solve(Ruleset &r, Letterset &letters, size_t steps)
{
	while (steps-->0)
	{
		for (auto& [_, v] : r)
		{
			v.other = v.count;
			v.count = 0;
		}
		for (auto& [k, v]: r)
		{
			if (!v.other)
			{
				// empty
			}
			else if (!v.insert)
			{
				// doesn't transform
				v.count += v.other;
			}
			else
			{
				r[make_pair(k.first, v.insert)].count += v.other;
				r[make_pair(v.insert, k.second)].count += v.other;
				letters[v.insert] += v.other;
			}
		}
	}

	size_t max = 0;
	size_t min = SIZE_MAX;
	for (auto [_, x] : letters)
	{
		if (!x) { continue; }
		if (min > x) { min = x; }
		if (max < x) { max = x; }
	}
	return min < max ?  max - min : 0;
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

	Letterset letterset;
	Ruleset ruleset;
	parse_problem(in, ruleset, letterset);
	in.close();

	fmt::print("Part1: {}\n", solve(ruleset, letterset, 10));
	fmt::print("Part2: {}\n", solve(ruleset, letterset, 30));

	return 0;
}
