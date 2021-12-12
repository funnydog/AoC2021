#include <algorithm>
#include <cctype>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <fmt/format.h>

using namespace std;

using Map = unordered_map<string, vector<string>>;

istream& operator>>(istream& in, Map& m)
{
	m.clear();
	string src, dst;
	while (getline(in, src, '-') && getline(in, dst))
	{
		m[src].push_back(dst);
		m[dst].push_back(src);
	}
	return in;
}

static size_t backtrack(const string& cur, vector<string>& lst, bool assigned, Map& m)
{
	if (cur == "end")
	{
		return 1;
	}

	lst.push_back(cur);
	size_t count = 0;
	for (const auto& can : m[cur])
	{
		bool should_skip = false;
		if (can[0] != toupper(can[0]))
		{
			should_skip = find(lst.cbegin(), lst.cend(), can) != lst.cend();
		}

		if ((should_skip && assigned) || can == "start")
		{
			continue;
		}

		count += backtrack(can, lst, assigned || should_skip, m);
	}
	lst.pop_back();
	return count;
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

	Map m;
	in >> m;
	in.close();

	vector<string> lst;
	fmt::print("Part1: {}\n", backtrack("start", lst, true, m));
	fmt::print("Part2: {}\n", backtrack("start", lst, false, m));
	return 0;
}
