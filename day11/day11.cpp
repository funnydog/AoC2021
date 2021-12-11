#include <fstream>
#include <vector>
#include <fmt/format.h>

using namespace std;

static const pair<int, int> NEIGHBORS[] = {
	{-1, 0},
	{-1, -1},
	{0, -1},
	{1, -1},
	{1, 0},
	{1, 1},
	{0, 1},
	{-1, 1},
};

struct Map
{
	char data[10][10];

	void inc(int x, int y)
	{
		if (data[y][x] > 0 && ++data[y][x] > '9')
		{
			data[y][x] = 0;
			for (auto [dx, dy]: NEIGHBORS)
			{
				int nx = x + dx;
				int ny = y + dy;
				if (0 <= nx && nx < 10 && 0 <= ny && ny < 10)
				{
					inc(nx, ny);
				}
			}
		}
	}

	size_t step()
	{
		for (int y = 0; y < 10; y++)
		{
			for (int x = 0; x < 10; x++)
			{
				inc(x, y);
			}
		}

		size_t count = 0;
		for (int y = 0; y < 10; y++)
		{
			for (int x = 0; x < 10; x++)
			{
				if (data[y][x] == 0)
				{
					data[y][x] = '0';
					count++;
				}
			}
		}
		return count;
	}
};

istream& operator>>(istream& in, Map &m)
{
	int y = 0;
	int x = 0;
	char ch;
	while (y < 10 && in.get(ch))
	{
		if (ch == '\n')
		{
			y++;
			x = 0;
		}
		else if (x < 10)
		{
			m.data[y][x] = ch;
			x++;
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
	in.close();

	size_t part1 = 0;
	size_t part2 = 0;
	while (1)
	{
		size_t flashes = m.step();
		if (part2 < 100)
		{
			part1 += flashes;
		}
		part2++;
		if (flashes == 100)
		{
			break;
		}
	}

	fmt::print("Part1: {}\n", part1);
	fmt::print("Part2: {}\n", part2);
	return 0;
}
