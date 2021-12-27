#include <cassert>
#include <algorithm>
#include <fstream>
#include <list>
#include <queue>
#include <unordered_map>
#include <vector>
#include <fmt/format.h>

using namespace std;

static const unsigned ENERGY[] = { 1, 10, 100, 1000 };

struct Point
{
	int x;
	int y;
};

static vector<Point> points;

struct Map
{
	char data[7][13];
	unsigned energy;
	unsigned priority;

	bool complete() const
	{
		for (int y = 2; y < 7; y++)
		{
			for (int i = 0; i < 4; i++)
			{
				if (data[y][3 + i * 2] == '#')
				{
					return true;
				}
				if (data[y][3 + i * 2] != 'A' + i)
				{
					return false;
				}
			}
		}
		return true;
	}

	bool can_move(Point p) const
	{
		if (p.y == 1)
		{
			return true;
		}
		if (data[p.y-1][p.x] != '.')
		{
			return false;
		}
		int e = data[p.y][p.x];
		if (p.x !=  3 + 2 * (e - 'A'))
		{
			return true;
		}
		for (int y = 5; y > p.y; y--)
		{
			if (data[y][p.x] == '#')
			{
			}
			else if (data[y][p.x] != e)
			{
				return true;
			}
		}
		return false;
	}

	void set_priority()
	{
		unsigned prio = 0;
		for (auto [x, y]: points)
		{
			int e = data[y][x];
			if ('A' <= e && e <= 'D')
			{
				int tx = 3 + 2 * (e - 'A');
				prio += (tx > x ? (tx - x) : (x - tx)) * ENERGY[e-'A'];
			}
		}
		priority = energy + prio;
	}

	void insert_at(int y, string row)
	{
		size_t len = row.size();
		if (len > 13)
		{
			len = 13;
		}
		memmove(data[y+1], data[y], (7 - y- 1) * 13);
		memmove(data[y], row.data(), len);
	}

	void build_points(vector<Point>& points) const
	{
		points.clear();
		for (int y = 0; y < 7; y++)
		{
			for (int x = 0; x < 13; x++)
			{
				if (y == 1 && (x == 3 || x == 5 || x == 7 || x == 9))
				{
					continue;
				}
				switch (data[y][x])
				{
				case 'A':
				case 'B':
				case 'C':
				case 'D':
				case '.':
					points.push_back({x, y});
				default:
					break;
				}
			}
		}
	}

	bool operator==(const Map& other) const
	{
		return !memcmp(data, other.data, sizeof(data));
	}
};

istream& operator>>(istream& in, Map& m)
{
	m.energy = 0;
	memset(m.data, '#', sizeof(m.data));

	int y = 0;
	int x = 0;
	char ch;
	while (in.get(ch) && y < 7)
	{
		switch (ch)
		{
		case '.':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
			if (x < 13)
			{
				m.data[y][x] = ch;
			}
		case ' ':
		case '#': x++; break;
		case '\n': y++; x = 0; break;
		}
	}
	return in;
}

namespace std
{
	template<>
	struct hash<Map>
	{
		size_t operator()(const Map& m) const
		{
			size_t hash = 5381;
			for (auto [x, y]: points)
			{
				hash = (hash * 33) ^ m.data[y][x];
			}
			return hash;
		}
	};
}

struct MapCmp
{
	bool operator()(const Map& a, const Map& b) const
	{
		return a.priority > b.priority;
	}
};

using Dict = unordered_map<Map, unsigned>;
using Heap = priority_queue<Map, vector<Map>, MapCmp>;
using List = list<Map>;

static void find_home(const Map& m, int e, Point p, vector<Point>& dst)
{
	int x = 3 + 2 * (e - 'A');

	// walk left/right
	int dx = p.x < x ? 1 : -1;
	while (p.x != x)
	{
		p.x += dx;
		if (m.data[p.y][p.x] != '.')
		{
			return;
		}
	}

	// check vertically
	for (p.y = 5; p.y >= 2; p.y--)
	{
		int v = m.data[p.y][p.x];
		if (v == '#' || v == e)
		{
			// ignore
		}
		else if (v == '.')
		{
			dst.push_back(p);
			break;
		}
		else
		{
			break;
		}
	}
}

static void find_hallways(const Map& m, int e, Point p, vector<Point>& dst)
{
	// walk up
	for (p.y--; p.y > 1; p.y--)
	{
		if (m.data[p.y][p.x] != '.')
		{
			return;
		}
	}

	// walk left
	int x = p.x;
	for (p.x = x - 1; ; p.x--)
	{
		if (m.data[p.y][p.x] != '.')
		{
			break;
		}
		else if (p.x != 3 && p.x != 5 && p.x != 7 && p.x != 9)
		{
			dst.push_back(p);
		}
	}

	// walk right
	for (p.x = x + 1; ; p.x++)
	{
		if (m.data[p.y][p.x] != '.')
		{
			break;
		}
		else if (p.x != 3 && p.x != 5 && p.x != 7 && p.x != 9)
		{
			dst.push_back(p);
		}
	}
}

static unsigned manhattan(Point a, Point b)
{
	return (a.x > b.x ? (a.x - b.x) : (b.x - a.x))
		+ (a.y > b.y ? (a.y - b.y) : (b.y - a.y));
}

static List decisions(const Map& m)
{
	List res;
	vector<Point> dst;
	for (auto from : points)
	{
		int e = m.data[from.y][from.x];
		if ('A' <= e && e <= 'D' && m.can_move(from))
		{
			dst.clear();
			if (from.y == 1)
			{
				find_home(m, e, from, dst);
			}
			else
			{
				find_hallways(m, e, from, dst);
			}

			for (auto to: dst)
			{
				Map n(m);
				n.data[from.y][from.x] = '.';
				n.data[to.y][to.x] = e;
				n.energy += manhattan(from, to) * ENERGY[e - 'A'];
				res.push_back(move(n));
			}
		}
	}
	return res;
}

static size_t astar(const Map& start)
{
	size_t result = 0;
	Dict visited;
	Heap queue;

	start.build_points(points);
	visited[start] = 0;
	queue.push(start);
	while (!queue.empty())
	{
		Map p = queue.top();
		queue.pop();

		if (p.complete())
		{
			result = p.energy;
			break;
		}

		for (auto &c : decisions(p))
		{
			auto it = visited.find(c);
			if (it == visited.end() || c.energy < it->second)
			{
				c.set_priority();
				visited[c] = c.energy;
				queue.push(c);
			}
		}
	}

	return result;
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

	fmt::print("Part1: {}\n", astar(m));

	m.insert_at(3, "  #D#C#B#A");
	m.insert_at(4, "  #D#B#A#C");
	fmt::print("Part2: {}\n", astar(m));
	return 0;
}
