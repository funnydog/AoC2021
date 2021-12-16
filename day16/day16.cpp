#include <cctype>
#include <fstream>
#include <memory>
#include <numeric>
#include <vector>
#include <fmt/format.h>

using namespace std;

class Bitstream
{
public:
	explicit Bitstream(const string& str)
		: cur(str.cbegin()),
		  end(str.cend()),
		  rawdata(0),
		  available(0),
		  pos(0)
	{}

	unsigned get(int needed)
	{
		while (available < needed)
		{
			if (cur == end)
			{
				fmt::print(stderr, "EOF\n");
				exit(1);
			}
			int val = *cur++;
			if (isdigit(val))
			{
				val -= '0';
			}
			else if ('A' <= val && val <= 'F')
			{
				val -= 'A' - 10;
			}
			else
			{
				continue;
			}
			rawdata = rawdata << 4 | val;
			available += 4;
		}

		pos += needed;
		available -= needed;

		unsigned value = rawdata >> available;
		rawdata &= (1U<<available) - 1;
		return value;
	}

	size_t getPos() const
	{
		return pos;
	}

private:
	string::const_iterator cur;
	string::const_iterator end;
	unsigned rawdata;
	int available;
	size_t pos;
};

struct Node
{
	int version;
	int type;
	uint64_t value;
	vector<unique_ptr<Node>> children;
};

static unique_ptr<Node> parse(Bitstream& bs)
{
	auto node = make_unique<Node>();

	node->version = bs.get(3);
	node->type = bs.get(3);
	if (node->type == 4)
	{
		uint64_t value = 0;
		unsigned group;
		do
		{
			group = bs.get(5);
			value = value << 4 | (group & 0xF);
		} while (group & 0x10);
		node->value = value;
	}
	else if (bs.get(1))
	{
		size_t count = bs.get(11);
		while (count-->0)
		{
			node->children.push_back(parse(bs));
		}
	}
	else
	{
		size_t end = bs.get(15);
		end += bs.getPos();
		while (bs.getPos() < end)
		{
			node->children.push_back(parse(bs));
		}
	}
	return node;
}

static int interp1(const Node& node)
{
	if (node.type == 4)
	{
		return node.version;
	}

	return accumulate(
		node.children.cbegin(),
		node.children.cend(),
		node.version,
		[](auto acc, const auto& node)
		{
			return acc + interp1(*node);
		});
}

static uint64_t interp2(const Node& node)
{
	uint64_t value;
	switch (node.type)
	{
	case 0:
		value = accumulate(
			node.children.cbegin(),
			node.children.cend(),
			0ULL,
			[](auto acc, const auto& node)
			{
				return acc + interp2(*node);
			});
		break;

	case 1:
		value = accumulate(
			node.children.cbegin(),
			node.children.cend(),
			1ULL,
			[](auto acc, const auto& node)
			{
				return acc * interp2(*node);
			});
		break;

	case 2:
		value = UINT64_MAX;
		for (auto& n: node.children)
		{
			uint64_t tmp = interp2(*n);
			if (value > tmp)
			{
				value = tmp;
			}
		}
		break;

	case 3:
		value = 0;
		for (auto& n: node.children)
		{
			uint64_t tmp = interp2(*n);
			if (value < tmp)
			{
				value = tmp;
			}
		}
		break;

	case 4:
		value = node.value;
		break;

	case 5:
		value = interp2(*node.children[0]) > interp2(*node.children[1]);
		break;

	case 6:
		value = interp2(*node.children[0]) < interp2(*node.children[1]);
		break;

	case 7:
		value = interp2(*node.children[0]) == interp2(*node.children[1]);
		break;

	default:
		fmt::print(stderr, "Unknown Type ID {}\n", node.type);
		exit(1);
	}
	return value;
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

	string line;
	while (getline(in, line))
	{
		auto bs = Bitstream(line);
		auto program = parse(bs);
		fmt::print("Part1: {}\n", interp1(*program));
		fmt::print("Part2: {}\n", interp2(*program));
	}
	in.close();
	return 0;
}
