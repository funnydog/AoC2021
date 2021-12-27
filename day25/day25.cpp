#include <algorithm>
#include <fstream>
#include <vector>
#include <fmt/format.h>

using namespace std;

struct Map
{
	array<string, 3> data;
	size_t width, height;
	unsigned cur;

	void single_step(int herd, unsigned dx, unsigned dy)
	{
		const string& src = data[cur];
		if (++cur == 3)
		{
			cur = 0;
		}
		string& dst = data[cur];

		fill(dst.begin(), dst.end(), '.');
		for (size_t y = 0, off = 0; y < height; y++)
		{
			for (size_t x = 0; x < width; x++, off++)
			{
				if (src.at(off) == '.')
				{
				}
				else if (src.at(off) != herd)
				{
					dst[off]= src.at(off);
				}
				else
				{
					size_t nx = x;
					if ((nx += dx) == width)
					{
						nx = 0;
					}
					size_t ny = y;
					if ((ny += dy) == height)
					{
						ny = 0;
					}
					size_t noff = ny * width + nx;
					if (src.at(noff) == '.')
					{
						dst[noff] = herd;
					}
					else
					{
						dst[off] = herd;
					}
				}
			}
		}
	}

	void step()
	{
		single_step('>', 1, 0);
		single_step('v', 0, 1);
	}

	bool deadlock() const
	{
		const string& a = data[cur];
		const string& b = data[(cur + 1) % 3];
		return !memcmp(a.data(), b.data(), a.size());
	}
};

istream& operator>>(istream& in, Map &m)
{
	m.width = m.height = 0;
	m.cur = 0;

	string line;
	while (getline(in, line))
	{
		if (!m.width)
		{
			m.width = line.size();
		}
		m.height++;
		for (auto c: line)
		{
			m.data[0].push_back(c);
		}
	}
	m.data[1].resize(m.width * m.height, '.');
	m.data[2].resize(m.width * m.height, '.');

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
	in.close();

	int i = 0;
	do
	{
		m.step();
		i++;
	} while (!m.deadlock());

	fmt::print("Part1: {}\n", i);
	return 0;
}
