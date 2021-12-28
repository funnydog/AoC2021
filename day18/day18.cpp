#include <algorithm>
#include <fstream>
#include <vector>
#include <memory>
#include <fmt/format.h>

using namespace std;

enum NodeType { PAIR, NUMBER };

struct Node
{
	NodeType type;
	int number;
	Node *up;
	unique_ptr<Node> left;
	unique_ptr<Node> right;

	Node(Node *parent, int number) :
		type(NUMBER),
		number(number),
		up(parent)
	{
	}

	explicit Node(Node *parent)
		: type(PAIR)
		, number(0)
		, up(parent)
	{
	}

	string to_string() const
	{
		if (type == PAIR)
		{
			return fmt::format(
				"[{},{}]",
				left->to_string(),
				right->to_string()
				);
		}
		else
		{
			return fmt::format("{}", number);
		}
	}
};

enum TokenType { LPAR, RPAR, COMMA, NUM, EOT, };

class Scanner
{
public:
	explicit Scanner(const string& str) :
		cur(str.cbegin()),
		end(str.cend()),
		type(EOT),
		value(0)
	{
		get();
	}

	void get()
	{
		value = 0;
		if (cur == end)
		{
			type = EOT;
			return;
		}
		switch (*cur)
		{
		case '[': ++cur; type = LPAR; break;
		case ']': ++cur; type = RPAR; break;
		case ',': ++cur; type = COMMA; break;
		default:
			if (isdigit(*cur))
			{
				while (cur != end && isdigit(*cur))
				{
					value = value * 10 + *cur - '0';
					++cur;
				}
				type = NUM;
			}
			else
			{
				cur = end;
				type = EOT;
			}
		}
	}

	void match(TokenType t)
	{
		if (type != t)
		{
			throw std::runtime_error(
				fmt::format("expected {}, found {}", t, type));
		}
		get();
	}

	TokenType getToken() const { return type; }
	int getValue() const { return value; }

private:
	string::const_iterator cur;
	string::const_iterator end;
	TokenType type;
	int value;
};

static unique_ptr<Node> parse_node(Scanner &s, Node *parent)
{
	if (s.getToken() == LPAR)
	{
		auto n = make_unique<Node>(parent);
		s.get();
		n->left = parse_node(s, n.get());
		s.match(COMMA);
		n->right = parse_node(s, n.get());
		s.match(RPAR);
		return n;
	}
	else
	{
		auto n = make_unique<Node>(parent, s.getValue());
		s.get();
		return n;
	}
}

static unique_ptr<Node> parse(const string& str)
{
	Scanner s(str);
	return parse_node(s, nullptr);
}

static void add_left(Node *n, int value)
{
	while (n->up && n->up->left.get() == n)
	{
		n = n->up;
	}

	if (n->up)
	{
		n = n->up->left.get();
		while (n && n->type == PAIR)
		{
			n = n->right.get();
		}
		if (n)
		{
			n->number += value;
		}
	}
}

static void add_right(Node *n, int value)
{
	while (n->up && n->up->right.get() == n)
	{
		n = n->up;
	}

	if (n->up)
	{
		n = n->up->right.get();
		while (n && n->type == PAIR)
		{
			n = n->left.get();
		}
		if (n)
		{
			n->number += value;
		}
	}
}

static void explode(Node& n, int lvl)
{
	if (lvl >= 4 && n.type == PAIR)
	{
		add_left(&n, n.left->number);
		add_right(&n, n.right->number);

		n.left.reset();
		n.right.reset();
		n.type = NUMBER;
		n.number = 0;
	}
	else if (n.type == PAIR)
	{
		explode(*n.left, lvl + 1);
		explode(*n.right, lvl + 1);
	}
	else
	{
	}
}

bool action;
static void split(Node& n, int lvl)
{
	if (action)
	{
		return;
	}
	else if (n.type == PAIR)
	{
		split(*n.left, lvl+1);
		split(*n.right, lvl+1);
	}
	else if (n.number >= 10)
	{
		n.type = PAIR;
		n.left = make_unique<Node>(&n, n.number / 2);
		n.right = make_unique<Node>(&n, (n.number + 1) / 2);
		explode(n, lvl);
		action = true;
	}
}

static void reduce(Node& n)
{
	explode(n, 0);
	do
	{
		action = false;
		split(n, 0);
	} while (action);
}

static unique_ptr<Node> clone(const Node& n, Node *parent)
{
	if (n.type == NUMBER)
	{
		return make_unique<Node>(parent, n.number);
	}
	else
	{
		auto c = make_unique<Node>(parent);
		c->left = clone(*n.left, c.get());
		c->right = clone(*n.right, c.get());
		return c;
	}
}

static unique_ptr<Node> add(const Node &a, const Node &b)
{
	auto n = make_unique<Node>(nullptr);
	n->left = clone(a, n.get());
	n->right = clone(b, n.get());
	reduce(*n);
	return n;
}

static int magnitude(const Node &n)
{
	if (n.type == NUMBER)
	{
		return n.number;
	}
	else
	{
		return 3 * magnitude(*n.left)
			+ 2 * magnitude(*n.right);
	}
}

static int part1(const vector<unique_ptr<Node>>& numbers)
{
	auto acc = clone(*numbers.at(0), nullptr);
	for (auto it = numbers.begin() + 1;
	     it != numbers.end();
	     ++it)
	{
		acc = add(*acc, **it);
	}
	return magnitude(*acc);
}

static int part2(const vector<unique_ptr<Node>>& numbers)
{
	int max = 0;
	for (auto i = numbers.begin();
	     i != numbers.end();
	     ++i)
	{
		for (auto j = numbers.begin();
		     j != numbers.end();
		     ++j)
		{
			if (i != j)
			{
				auto number = add(**i, **j);
				auto m = magnitude(*number);
				if (max < m)
				{
					max = m;
				}
			}
		}
	}
	return max;
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

	vector<unique_ptr<Node>> numbers;
	string line;
	while (getline(in, line))
	{
		numbers.push_back(parse(line));
	}
	in.close();

	fmt::print("Part1: {}\n", part1(numbers));
	fmt::print("Part2: {}\n", part2(numbers));
	return 0;
}
