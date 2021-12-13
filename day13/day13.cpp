#include <algorithm>
#include <fstream>
#include <vector>
#include <fmt/format.h>

using namespace std;

struct Map
{
	vector<pair<int, int>> data;
	int width;
	int height;

	void fold_up(int pos)
	{
		height = pos;
		for (auto& p: data)
		{
			if (p.second > pos)
			{
				p.second = 2 * pos - p.second;
			}
		}
	}

	void fold_left(int pos)
	{
		width = pos;
		for (auto& p: data)
		{
			if (p.first > pos)
			{
				p.first = 2 * pos - p.first;
			}
		}
	}

	size_t count(void)
	{
		sort();
		size_t count = 0;
		int px = width;
		int py = height;
		for (auto [x, y] : data)
		{
			if (x != px || y != py)
			{
				count++;
			}
			px = x;
			py = y;
		}
		return count;
	}

	void print(void)
	{
		sort();

		int px = -1;
		int py = 0;
		for (auto [x, y]: data)
		{
			if (x == px && y == py)
			{
				continue;
			}

			for (; py < y; py++)
			{
				putchar('\n');
				px = -1;
			}

			for (px++; px < x; px++)
			{
				putchar(' ');
			}

			putchar('#');
		}
		putchar('\n');
	}

	void sort(void)
	{
		std::sort(data.begin(), data.end(), [](const auto& a, const auto &b) {
			return (a.second != b.second)
				? (a.second < b.second)
				: (a.first < b.first);
		});
	}
};

istream& operator>>(istream& in, Map &m)
{
	m.data.clear();
	m.width = m.height = 0;

	string line;
	while (getline(in, line) && !line.empty())
	{
		auto p = line.find(',');
		if (p != string::npos)
		{
			int x = stoi(line.substr(0, p));
			int y = stoi(line.substr(p+1));
			m.data.push_back(make_pair(x, y));
			if (m.width <= x)
			{
				m.width = x + 1;
			}
			if (m.height <= y)
			{
				m.height = y + 1;
			}
		}
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

	Map m;
	in >> m;

	size_t part1 = SIZE_MAX;
	string cmd;
	while (getline(in, cmd))
	{
		auto sep = cmd.find("=");
		if (sep != string::npos)
		{
			int pos = stoi(cmd.substr(sep+1));

			switch(cmd.at(sep - 1))
			{
			case 'x': m.fold_left(pos); break;
			case 'y': m.fold_up(pos); break;
			}
		}
		if (part1 == SIZE_MAX)
		{
			part1 = m.count();
		}
	}
	in.close();

	fmt::print("Part1: {}\n", part1);
	fmt::print("Part2:\n");
	m.print();
	return 0;
}
