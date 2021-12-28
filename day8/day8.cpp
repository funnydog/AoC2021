#include <algorithm>
#include <numeric>
#include <fstream>
#include <vector>
#include <fmt/format.h>

using namespace std;

// entry in the input
struct Entry
{
	unsigned wiring[10];
	unsigned display[4];
};

// for a given number of segments turned on
// provide a list of potential digits
static vector<unsigned> candidates[8];

// number of common segments for two digits
static int inters[10][10];

static unsigned encode(const string& str)
{
	unsigned value = 0;
	for (auto s: str)
	{
		value |= 1U<<(s-'a');
	}
	return value;
}

istream& operator>>(istream& in, Entry& e)
{
	string tmp;
	for (int i = 0; i < 10; ++i)
	{
		in >> tmp;
		e.wiring[i] = encode(tmp);
	}
	in >> tmp;		// |
	for (int i = 0; i < 4; i++)
	{
		in >> tmp;
		e.display[i] = encode(tmp);
	}
	return in;
}

static int bitcount(unsigned x)
{
	int count = 0;
	while (x)
	{
		x &= x - 1;
		count++;
	}
	return count;
}

static unsigned part1(const vector<Entry>& entries)
{
	unsigned count = 0;
	for (const auto& e : entries)
	{
		for (const auto &d: e.display)
		{
			if (candidates[bitcount(d)].size() == 1)
			{
				count++;
			}
		}
	}
	return count;
}

static unsigned solve(const Entry& entry)
{
	// find each digit find the list of wiring candidates
	vector<unsigned> digits[10];
	for (auto& w: entry.wiring)
	{
		int bc = bitcount(w);
		for (auto i: candidates[bc])
		{
			digits[i].push_back(w);
		}
	}

	// order the digits by the number of candidates
	size_t ordered[10];
	for (size_t i = 0; i < 10; i++)
	{
		ordered[i] = i;
	}
	sort(ordered, ordered + 10, [&digits](int i, int j)
	{
		return digits[i].size() < digits[j].size();
	});

	// find the index where the candidates > 1
	size_t t = 0;
	while (digits[ordered[t]].size() <= 1)
	{
		t++;
	}

	// remove the wrong candidates
	for (size_t i = 0; i < t; i++)
	{
		unsigned a = ordered[i];
		unsigned aseg = digits[a][0];

		for (size_t j = t; j < 10; j++)
		{
			unsigned b = ordered[j];
			for (auto it = digits[b].begin(); it != digits[b].end();)
			{
				int inter = bitcount(aseg & *it);
				if (inter != inters[a][b])
				{
					it = digits[b].erase(it);
				}
				else
				{
					++it;
				}
			}
			if (digits[b].size() == 1)
			{
				swap(ordered[j], ordered[t]);
				t++;
			}
		}
	}

	if (t < 10)
	{
		fmt::print(stderr, "Couldn't find a solution\n");
		return 0;
	}

	// map the encoded segment value to a digit
	unsigned map[1U<<8];
	for (size_t i = 0; i < 10; i++)
	{
		map[digits[ordered[i]][0]] = ordered[i];
	}

	// compute the result
	return accumulate(
		entry.display, entry.display+4, 0,
		[map](size_t acc, unsigned e)
		{
			return acc * 10 + map[e];
		});
}

static size_t part2(const vector<Entry>& entries)
{
	return accumulate(entries.cbegin(),
			  entries.cend(),
			  0,
			  [](size_t acc, const Entry& e)
			  {
				  return acc + solve(e);
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

	vector<Entry> entries;
	Entry e;
	while (in>>e)
	{
		entries.push_back(move(e));
	}
	in.close();

	static const string segs[10] = {
		"abcefg",
		"cf",
		"acdeg",
		"acdfg",
		"bcdf",
		"abdfg",
		"abdefg",
		"acf",
		"abcdefg",
		"abcdfg",
	};

	// prepare the data
	for (size_t i = 0; i < 10; ++i)
	{
		candidates[segs[i].size()].push_back(i);
		for (size_t j = 0; j < 10; ++j)
		{
			inters[i][j] = bitcount(
				encode(segs[i]) & encode(segs[j]));
		}
	}

	fmt::print("Part1: {}\n", part1(entries));
	fmt::print("Part2: {}\n", part2(entries));
	return 0;
}
