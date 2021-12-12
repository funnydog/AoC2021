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

static size_t backtrack(vector<string>& lst, bool assigned, const Map& m)
{
	if (lst.back() == "end")
	{
		return 1;
	}

	size_t count = 0;
	for (const auto& can : m.at(lst.back()))
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

		lst.push_back(can);
		count += backtrack(lst, assigned || should_skip, m);
		lst.pop_back();
	}
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
	lst.push_back("start");
	fmt::print("Part1: {}\n", backtrack(lst, true, m));
	fmt::print("Part2: {}\n", backtrack(lst, false, m));
	return 0;
}
