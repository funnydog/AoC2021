#include <fstream>
#include <fmt/format.h>
#include <vector>

using namespace std;

enum class Cmd
{
	FORWARD,
	DOWN,
	UP
};

struct Ins
{
	Cmd cmd;
	int arg;
};

istream& operator>>(istream& in, Ins &instr)
{
	string s;
	if (!(in >> s))
	{
		return in;
	}
	else if (s == "down")
	{
		instr.cmd = Cmd::DOWN;
	}
	else if (s == "up")
	{
		instr.cmd = Cmd::UP;
	}
	else if (s == "forward")
	{
		instr.cmd = Cmd::FORWARD;
	}
	else
	{
		in.setstate(ios::failbit);
		return in;
	}
	return in >> instr.arg;
}

static int part1(const vector<Ins>& program)
{
	int x = 0;
	int depth = 0;

	for (auto [cmd, arg] : program)
	{
		switch (cmd)
		{
		case Cmd::FORWARD: x += arg; break;
		case Cmd::DOWN: depth += arg; break;
		case Cmd::UP: depth -= arg; break;
		}
	}

	return x * depth;
}

static int part2(const vector<Ins>& program)
{
	int x = 0;
	int depth = 0;
	int aim = 0;

	for (auto [cmd, arg] : program)
	{
		switch (cmd)
		{
		case Cmd::FORWARD:
			x += arg;
			depth += arg * aim;
			break;

		case Cmd::DOWN:
			aim += arg;
			break;

		case Cmd::UP:
			aim -= arg;
			break;
		}
	}

	return x * depth;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fmt::print(stderr, "Usage: {} <filaname>\n", argv[0]);
		return 1;
	}

	ifstream in(argv[1]);
	if (!in)
	{
		fmt::print(stderr, "Cannot open {}\n", argv[1]);
		return 1;
	}

	vector<Ins> program;
	Ins tmp;
	while (in >> tmp)
	{
		program.emplace_back(tmp);
	}
	in.close();

	fmt::print("Part1: {}\n", part1(program));
	fmt::print("Part2: {}\n", part2(program));

	return 0;
}
