#include <fstream>
#include <vector>
#include <unordered_map>

#include <fmt/format.h>

using namespace std;

struct Segment
{
	int x0, y0;
	int x1, y1;
};

istream& operator>>(istream& in, Segment& s)
{
	in >> s.x0;
	in.get();		// ,
	in >> s.y0;
	in.seekg(4, ios_base::cur); // " => "
	in >> s.x1;
	in.get();		// ,
	in >> s.y1;
	return in;
}

class SparseMap
{
public:
	void draw(int x0, int y0, int x1, int y1)
	{
		int sx = x0 < x1 ? 1 : x0 > x1 ? -1 : 0;
		int sy = y0 < y1 ? 1 : y0 > y1 ? -1 : 0;
		while (true)
		{
			map[pair<int,int>(x0, y0)]++;
			if (x0 == x1 && y0 == y1)
			{
				break;
			}
			x0 += sx;
			y0 += sy;
		}
	}

	size_t count() const
	{
		size_t count = 0;
		for (const auto& el : map)
		{
			if (el.second > 1)
			{
				count++;
			}
		}
		return count;
	}

	void reset()
	{
		map.clear();
	}

private:
	struct HashPoint
	{
		size_t operator()(const pair<int,int>& k) const
		{
			return k.first + (k.second<<8);
		}
	};
	unordered_map<pair<int,int>, size_t, HashPoint> map;
};

static size_t part1(SparseMap &map, const vector<Segment>& segs)
{
	map.reset();
	for (const auto& s : segs)
	{
		if (s.x0 == s.x1 || s.y0 == s.y1)
		{
			map.draw(s.x0, s.y0, s.x1, s.y1);
		}
	}
	return map.count();
}

static size_t part2(SparseMap &map, const vector<Segment>& segs)
{
	map.reset();
	for (const auto& s : segs)
	{
		map.draw(s.x0, s.y0, s.x1, s.y1);
	}
	return map.count();
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

	vector<Segment> segs;
	Segment tmp;
	while (in >> tmp)
	{
		segs.push_back(move(tmp));
	}
	in.close();

	SparseMap map;
	fmt::print("Part1: {}\n", part1(map, segs));
	fmt::print("Part2: {}\n", part2(map, segs));
	return 0;
}
