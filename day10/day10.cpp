#include <algorithm>
#include <fstream>
#include <vector>
#include <fmt/format.h>

using namespace std;

static const string OPENED = "([{<";
static const string CLOSED = ")]}>";
static const size_t ERROR_POINTS[] = { 3, 57, 1197, 25137 };

static size_t check_line(const string& line, string& stack)
{
	stack.clear();
	for (auto c : line)
	{
		auto idx = OPENED.find(c);
		if (idx != string::npos)
		{
			stack.push_back(idx);
		}
		else
		{
			if (c != CLOSED[stack.back()])
			{
				return ERROR_POINTS[CLOSED.find(c)];
			}
			stack.pop_back();
		}
	}
	return 0;
}

static pair<size_t, size_t> solve(const vector<string>& lines)
{
	size_t sum = 0;
	vector<size_t> completions;

	string stack;
	for (const auto& line : lines)
	{
		size_t points = check_line(line, stack);
		if (points)
		{
			sum += points;
		}
		else
		{
			size_t score = 0;
			for (auto it = stack.rbegin(); it != stack.rend(); ++it)
			{
				score = score * 5 + *it + 1;
			}
			completions.push_back(score);
		}
	}

	sort(completions.begin(), completions.end());

	return make_pair(sum, completions[completions.size()/2]);
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

	vector<string> lines;
	string line;
	while (getline(in, line))
	{
		lines.push_back(move(line));
	}
	in.close();

	auto [part1, part2] = solve(lines);
	fmt::print("Part1: {}\n", part1);
	fmt::print("Part2: {}\n", part2);

	return 0;
}
