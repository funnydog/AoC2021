#include <cmath>
#include <fstream>
#include <tuple>
#include <fmt/format.h>

using namespace std;

using Area = tuple<int,int,int,int>;

istream& operator>>(istream& in, Area &ar)
{
	in.seekg(15, ios::cur);
	in >> get<0>(ar);
	in.seekg(2, ios::cur);
	in >> get<1>(ar);
	in.seekg(4, ios::cur);
	in >> get<2>(ar);
	in.seekg(2, ios::cur);
	in >> get<3>(ar);
	return in;
}

static int simx(const Area &a, int vx)
{
	int x = 0;
	for (;;)
	{
		x += vx;
		if (vx)
		{
			vx--;
		}
		if (x > get<1>(a))
		{
			return 0;
		}
		else if (get<0>(a) <= x)
		{
			return 1;
		}
		else if (vx == 0)
		{
			return 0;
		}
	}
}

static int sim(const Area& a, int vx, int vy)
{
	int x = 0;
	int y = 0;

	/* boost the calculation if it's coming back */
	if (vy >= 0)
	{
		int t = 2 * vy + 1;
		vy = -vy - 1;
		x = (vx + 1) * vx / 2;
		if (vx > t)
		{
			vx -= t;
		}
		else
		{
			vx = 0;
		}
		x -= (vx + 1) * vx / 2;
	}

	for (;;)
	{
		x += vx;
		y += vy;
		if (vx)
		{
			vx--;
		}
		vy--;
		if (x > get<1>(a) || y < get<2>(a))
		{
			return 0;
		}
		else if (get<0>(a) <= x && y <= get<3>(a))
		{
			return 1;
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

	Area a;
	in>>a;
	in.close();

	auto [x0, x1, y0, y1] = a;

	int vy_max = (y0 + 1) * y0 / 2;
	fmt::print("Part1: {}\n", vy_max);

	int vy_min = y0;
	int vx_max = x1;
	int vx_min = ceill((sqrt(1 + 8 * x0) - 1) / 2);
	size_t count = 0;
	for (int vx = vx_min; vx <= vx_max; vx++)
	{
		if (!simx(a, vx))
		{
			continue;
		}
		for (int vy = vy_min; vy <= vy_max; vy++)
		{
			if (sim(a, vx, vy))
			{
				count++;
			}
		}
	}
	fmt::print("Part2: {}\n", count);
	return 0;
}
