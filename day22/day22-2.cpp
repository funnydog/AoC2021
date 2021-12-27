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
	Vec3 p[2];

	bool inside(const Cuboid& o) const
	{
		for (int i = 0; i < 3; i++)
		{
			if (p[0].v[i] < o.p[0].v[i] || p[1].v[i] > o.p[1].v[i])
			{
				return false;
			}
		}
		return true;
	}

	bool intersect(const Cuboid& o) const
	{
		for (int i = 0; i < 3; i++)
		{
			if (p[1].v[i] <= o.p[0].v[i] || p[0].v[i] >= o.p[1].v[i])
			{
				return false;
			}
		}
		return true;
	}

	uint64_t volume() const
	{
		uint64_t v = 1;
		for (int i = 0; i < 3; i++)
		{
			v *= p[1].v[i] - p[0].v[i];
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
			in >> tmp.c.p[0].v[i];
			in.seekg(2, ios::cur);
			in >> tmp.c.p[1].v[i];
			tmp.c.p[1].v[i]++;
		}
		in.get();
		ins.push_back(move(tmp));
	}
	return in;
}

static vector<Cuboid> split(const Cuboid& a, const Cuboid& b)
{
	vector<Cuboid> res;
	vector<Cuboid> tmp;

	res.push_back(a);
	for (int j = 0; j < 2; j++)
	{
		for (int i = 0; i < 3; i++)
		{
			auto& p = b.p[j];
			for (auto& c : res)
			{
				if (c.intersect(b)
				    && c.p[0].v[i] <= p.v[i]
				    && p.v[i] < c.p[1].v[i])
				{
					Cuboid n(c);
					n.p[1].v[i] = p.v[i];
					if (!n.inside(b))
					{
						tmp.push_back(n);
					}
					n = c;
					n.p[0].v[i] = p.v[i];
					if (!n.inside(b))
					{
						tmp.push_back(n);
					}
				}
				else
				{
					tmp.push_back(c);
				}
			}
			swap(res, tmp);
			tmp.clear();
		}
	}
	return res;
}

static void sub(vector<Cuboid>& lst, const Cuboid& b)
{
	vector<Cuboid> tmp;
	for (auto it = lst.begin(); it != lst.end(); )
	{
		if (it->inside(b))
		{
			it = lst.erase(it);
		}
		else if (!it->intersect(b))
		{
			++it;
		}
		else
		{
			auto r = split(*it, b);
			copy(r.begin(), r.end(), back_inserter(tmp));
			it = lst.erase(it);
		}
	}
	copy(tmp.begin(), tmp.end(), back_inserter(lst));
}

static void add(vector<Cuboid>& lst, const Cuboid& b)
{
	sub(lst, b);
	lst.push_back(b);
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

	Cuboid region{{{-50,-50,-50},{51,51,51}}};
	uint64_t part1 = 0;
	vector<Cuboid> lst;
	for (auto& ins: prog)
	{
		if (!part1 && !ins.c.inside(region))
		{
			part1 = volume(lst);
		}
		switch (ins.cmd)
		{
		case Instr::ON: add(lst, ins.c); break;
		case Instr::OFF: sub(lst, ins.c); break;
		}
	}

	fmt::print("Part1: {}\n", part1);
	fmt::print("Part2: {}\n", volume(lst));

	return 0;
}
