#include <algorithm>
#include <climits>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <fmt/format.h>

using namespace std;

namespace std
{
	template <> struct hash<pair<int, int>>
	{
		size_t operator()(const pair<int,int>& k) const
		{
			return 5381 + k.first * 101 + k.second;
		}
	};
}

struct Map
{
	unordered_map<pair<int, int>, int> data;
	unordered_map<pair<int, int>, int> swap;
	int background;
	int xmin, xmax;
	int ymin, ymax;

	void enhance(const string& algo)
	{
		int nbg = background == '#' ? algo.back() : algo.front();

		int nxmin, nymin, nxmax, nymax;
		nxmin = nymin = INT_MAX;
		nxmax = nymax = INT_MIN;
		swap.clear();
		for (int y = ymin-2; y <= ymax+2; y++)
		{
			for (int x = xmin-2; x <= xmax+2; x++)
			{
				unsigned idx = 0;
				for (int ny = y-1; ny <= y+1; ny++)
				{
					for (int nx = x-1; nx <= x+1; nx++)
					{
						idx <<= 1;
						auto it = data.find(make_pair(nx, ny));
						int val = it != data.end()
							? it->second
							: background;
						if (val == '#')
						{
							idx |= 1;
						}
					}
				}
				if (algo.at(idx) != nbg)
				{
					swap[make_pair(x, y)] = algo.at(idx);
					if (nxmin > x) nxmin = x;
					if (nxmax < x) nxmax = x;
					if (nymin > y) nymin = y;
					if (nymax < y) nymax = y;
				}
			}
		}
		data.swap(swap);
		background = nbg;
		xmin = nxmin;
		xmax = nxmax;
		ymin = nymin;
		ymax = nymax;
	}
};

istream& operator>>(istream& in, Map&m)
{
	m.data.clear();
	m.background = '.';
	int y = 0;
	string tmp;
	m.xmin = m.ymin = INT_MAX;
	m.xmax = m.ymax = INT_MIN;
	while (getline(in, tmp))
	{
		for (int x = 0; (unsigned)x < tmp.size(); x++)
		{
			if (tmp.at(x) == '#')
			{
				m.data[make_pair(x, y)] = '#';
				if (m.xmin > x) m.xmin = x;
				if (m.xmax < x) m.xmax = x;
				if (m.ymin > y) m.ymin = y;
				if (m.ymax < y) m.ymax = y;
			}
		}
		y++;
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

	string algo;
	getline(in, algo);
	in.get();

	Map m;
	in >> m;
	in.close();

	for (int i = 0; i < 2; i++)
	{
		m.enhance(algo);
	}
	fmt::print("Part1: {}\n", m.data.size());

	for (int i = 0; i < 48; i++)
	{
		m.enhance(algo);
	}
	fmt::print("Part2: {}\n", m.data.size());
	return 0;
}
