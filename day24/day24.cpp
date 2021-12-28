#include <algorithm>
#include <fstream>
#include <sstream>
#include <tuple>
#include <vector>
#include <fmt/format.h>

using namespace std;

static string load_program(istream& in)
{
	ostringstream ss;
	ss << in.rdbuf();
	return ss.str();
}

static vector<tuple<int, int, int>> constants(const string& str)
{
	vector<string> frags;
	size_t old = 0;
	size_t pos;
	while ((pos = str.find("inp", old + 3)) != string::npos)
	{
		frags.push_back(str.substr(old, pos));
		old = pos;
	}
	frags.push_back(str.substr(old));

	vector<size_t> lines;
	for (size_t i = 0; i < frags.size(); i++)
	{
		for (size_t j = 0; j < i; j++)
		{
			istringstream fa(frags[i]);
			istringstream fb(frags[j]);

			string sa, sb;
			for (size_t row = 0;
			     getline(fa, sa) && getline(fb, sb);
			     row++)
			{
				if (sa != sb && find(lines.cbegin(), lines.cend(), row) == lines.cend())
				{
					lines.push_back(row);
				}
			}
		}
	}

	vector<tuple<int,int,int>> res;
	for (const auto& f: frags)
	{
		istringstream ss(f);
		string line;
		int c[3];
		size_t cc = 0;
		for (size_t row = 0; getline(ss, line) && cc < 3; row++)
		{
			if (find(lines.cbegin(), lines.cend(), row) != lines.cend())
			{
				auto p = line.rfind(" ");
				if (p != string::npos)
				{
					c[cc++] = stoi(line.substr(p+1));
				}
			}
		}
		res.push_back(make_tuple(c[0], c[1], c[2]));
	}
	return res;
}

static int shot(int a, int b, int c, int d, int z)
{
	if (z % 26 + b != d)
	{
		return z / a * 26 + d + c;
	}
	else
	{
		return z / a;
	}
}

static uint64_t minvalue;
static uint64_t maxvalue;

static void backtrack(array<int, 14>& res, int k, int z, const vector<tuple<int,int,int>>& clist)
{
	if (k == 14)
	{
		if (z == 0)
		{
			minvalue = 0;
			for (auto p: res)
			{
				minvalue = minvalue * 10 + p;
			}
			if (!maxvalue)
			{
				maxvalue = minvalue;
			}
		}
	}
	else
	{
		auto [a, b, c] = clist[k];
		for (int d = 9; d > 0; d--)
		{
			if (b > 0 || z % 26 + b == d)
			{
				res[k] = d;
				backtrack(res, k+1, shot(a, b, c, d, z), clist);
			}
		}
	}
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

	string program = load_program(in);
	in.close();

	auto clist = constants(program);
	array<int, 14> res;
	backtrack(res, 0, 0, clist);
	fmt::print("Part1: {}\n", maxvalue);
	fmt::print("Part2: {}\n", minvalue);
	return 0;
}
