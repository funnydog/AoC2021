#include <algorithm>
#include <fstream>
#include <functional>
#include <queue>
#include <vector>
#include <fmt/format.h>

using namespace std;

struct Map
{
	vector<int> data;
	size_t width;
	size_t height;

	size_t dijkstra(size_t sx, size_t sy) const
	{
		static const pair<int,int> NEIGH[] = { {-1, 0}, {0, -1}, {1, 0}, {0, 1}};

		vector<int> riskmap(data.size(), 0);
		typedef tuple<int, size_t, size_t> ht;
		priority_queue<ht, vector<ht>, greater<ht>> queue;

		queue.push(make_tuple(0, sx, sy));
		while (!queue.empty())
		{
			auto [risk, x, y] = queue.top();
			queue.pop();

			for (auto [dx, dy]: NEIGH)
			{
				size_t nx = x + dx;
				size_t ny = y + dy;
				if (nx < width && ny < height)
				{
					size_t off = ny * width + nx;
					int nrisk = risk + data[off];
					if (!riskmap[off] || nrisk < riskmap[off])
					{
						riskmap[off] = nrisk;
						queue.push(make_tuple(nrisk, nx, ny));
					}
				}
			}
		}

		return riskmap.back();
	}

	void extend(int n)
	{
		vector<int> ndata(width * height * n * n, 0);
		auto it = ndata.begin();
		for (int ry = 0; ry < n; ry++)
		{
			for (size_t y = 0; y < height; y++)
			{
				for (int rx = 0; rx < n; rx++)
				{
					for (size_t x = 0; x < width; x++)
					{
						*it = (data[y * width + x] + rx + ry - 1) % 9 + 1;
						++it;
					}
				}
			}
		}
		width *= n;
		height *= n;
		data = ndata;
	}
};

istream& operator>>(istream& in, Map& m)
{
	m.data.clear();
	m.width = m.height = 0;

	char ch;
	while (in.get(ch))
	{
		if ('1' <= ch && ch <= '9')
		{
			if (!m.height) { m.width++; };
			m.data.push_back(ch - '0');
		}
		else if (ch == '\n')
		{
			m.height++;
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
	in>>m;
	in.close();

	fmt::print("Part1: {}\n", m.dijkstra(0, 0));
	m.extend(5);
	fmt::print("Part2: {}\n", m.dijkstra(0, 0));

	return 0;
}
