#include <algorithm>
#include <fstream>
#include <numeric>
#include <vector>
#include <fmt/format.h>

using namespace std;

struct Vec3
{
	int v[3];
};

struct Cuboid
{
	Vec3 p0, p1;

	bool inside(const Cuboid& o) const
	{
		for (int i = 0; i < 3; i++)
		{
			if (p0.v[i] < o.p0.v[i] || p1.v[i] > o.p1.v[i])
			{
				return false;
			}
		}
		return true;
	}

	bool intersect(const Cuboid& o, Cuboid& r) const
	{
		for (int i = 0; i < 3; i++)
		{
			auto v0 = p0.v[i] > o.p0.v[i] ? p0.v[i] : o.p0.v[i];
			auto v1 = p1.v[i] < o.p1.v[i] ? p1.v[i] : o.p1.v[i];
			if (v0 >= v1)
			{
				return false;
			}
			r.p0.v[i] = v0;
			r.p1.v[i] = v1;
		}
		return true;
	}

	uint64_t volume() const
	{
		uint64_t v = 1;
		for (int i = 0; i < 3; i++)
		{
			v *= p1.v[i] - p0.v[i];
		}
		return v;
	}
};

struct Instr
{
	enum { ON, OFF } cmd;
	Cuboid c;
};

istream& operator>>(istream& in, vector<Instr>& ins)
{
	ins.clear();
	while (in)
	{
		Instr tmp{};
		string cmd;
		in >> cmd;
		if (cmd == "on")
		{
			tmp.cmd = Instr::ON;
		}
		else if (cmd == "off")
		{
			tmp.cmd = Instr::OFF;
		}
		else
		{
			break;
		}

		for (int i = 0; i < 3; i++)
		{
			in.seekg(3, ios::cur);
			in >> tmp.c.p0.v[i];
			in.seekg(2, ios::cur);
			in >> tmp.c.p1.v[i];
			tmp.c.p1.v[i]++;
		}
		in.get();
		ins.push_back(move(tmp));
	}
	return in;
}

static void sub(vector<Cuboid>& on, vector<Cuboid>& off, const Cuboid& b)
{
	auto onsize = on.size();
	Cuboid inters;
	for (auto& c : off)
	{
		if (c.intersect(b, inters))
		{
			on.push_back(inters);
		}
	}

	for (size_t i = 0; i < onsize; i++)
	{
		if (on[i].intersect(b, inters))
		{
			off.push_back(inters);
		}
	}
}

static void add(vector<Cuboid>& on, vector<Cuboid>& off, const Cuboid& b)
{
	sub(on, off, b);
	on.push_back(b);
}

static uint64_t volume(const vector<Cuboid>& lst)
{
	return accumulate(lst.begin(), lst.end(), 0ULL, [](auto acc, auto &c){
		return acc + c.volume();
	});
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

	vector<Instr> prog;
	in >> prog;
	in.close();

	Cuboid region{{-50,-50,-50},{51,51,51}};
	uint64_t part1 = 0;
	vector<Cuboid> on, off;
	for (auto& ins: prog)
	{
		if (!part1 && !ins.c.inside(region))
		{
			part1 = volume(on) - volume(off);
		}
		switch (ins.cmd)
		{
		case Instr::ON: add(on, off, ins.c); break;
		case Instr::OFF: sub(on, off, ins.c); break;
		}
	}

	fmt::print("Part1: {}\n", part1);
	fmt::print("Part2: {}\n", volume(on) - volume(off));
	return 0;
}
