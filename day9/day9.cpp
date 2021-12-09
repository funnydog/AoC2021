#include <algorithm>
#include <fstream>
#include <vector>
#include <fmt/format.h>

using namespace std;

static const int DX[] = { -1, 0, 1, 0 };
static const int DY[] = { 0, -1, 0, 1 };

struct Map
{
	vector<string> data;
	vector<char> visited;
	size_t width;
	size_t height;

	size_t risk(int x, int y) const
	{
		int value = data[y][x];
		for (int i = 0; i < 4; i++)
		{
			int nx = x + DX[i];
			int ny = y + DY[i];
			if (0 <= nx && (size_t)nx < width
			    && 0 <= ny && (size_t)ny < height)
			{
				if (data[ny][nx] <= value)
				{
					return 0;
				}
			}
		}
		return 1 + value - '0';
	}

	size_t dfs(int x, int y)
	{
		if (data[y][x] == '9' || visited[y * width + x])
		{
			return 0;
		}

		visited[y * width + x] = 1;
		size_t size = 1;
		for (int i = 0; i < 4; i++)
		{
			int nx = x + DX[i];
			int ny = y + DY[i];
			if (0 <= nx && (size_t)nx < width
			    && 0 <= ny && (size_t)ny < height)
			{
				size += dfs(nx, ny);
			}
		}
		return size;
	}
};

static istream& operator>>(istream& in, Map& m)
{
	m.data.clear();
	m.width = m.height = 0;

	string str;
	while (getline(in, str))
	{
		if (m.data.empty())
		{
			m.width = str.size();
		}
		m.data.emplace_back(str);
		m.height++;
	}
	m.visited.resize(m.width * m.height, 0);
	return in;
}

static size_t part1(const Map &m)
{
	size_t risk = 0;
	for (size_t y = 0; y < m.height; y++)
	{
		for (size_t x = 0; x < m.width; x++)
		{
			risk += m.risk(x, y);
		}
	}
	return risk;
}

static size_t part2(Map &m)
{
	vector<size_t> basins;
	fill(m.visited.begin(), m.visited.end(), 0);
	for (size_t y = 0; y < m.height; y++)
	{
		for (size_t x = 0; x < m.width; x++)
		{
			size_t size = m.dfs(x, y);
			if (size)
			{
				basins.push_back(size);
			}
		}
	}

	sort(basins.begin(), basins.end(),
	     [](size_t a, size_t b) {
		     return b < a;
	     });

	auto it = basins.cbegin();
	size_t mul = 1;
	for (size_t i = 0; i < 3 && it != basins.cend(); ++i, ++it)
	{
		mul *= *it;
	}
	return mul;
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
	in>>m;
	in.close();

	fmt::print("Part1: {}\n", part1(m));
	fmt::print("Part2: {}\n", part2(m));

	return 0;
}
