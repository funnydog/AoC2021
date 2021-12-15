#include <fstream>
#include <vector>
#include <fmt/format.h>

using namespace std;

struct Report
{
	vector<unsigned> values;
	size_t bitlen;

	size_t power_consumption() const
	{
		vector<unsigned> ones(bitlen, 0);
		for (auto x: values)
		{
			size_t i = bitlen;
			while (i-->0)
			{
				ones[i] += x & 1;
				x >>= 1;
			}
		}
		unsigned gamma = 0;
		unsigned epsilon = 0;
		for (auto x: ones)
		{
			gamma <<= 1;
			epsilon <<= 1;
			if (x * 2 > values.size())
			{
				gamma |= 1;
			}
			else
			{
				epsilon |= 1;
			}
		}
		return gamma * epsilon;
	}

	unsigned filter(bool o2) const
	{
		unsigned mask = 0;
		unsigned value = 0;
		size_t k = bitlen;
		while (k-->0)
		{
			size_t zeros = 0;
			size_t count = 0;
			unsigned last = 0;
			for (auto x: values)
			{
				if ((x & mask) == value)
				{
					last = x;
					zeros += !(last & 1U<<k);
					count++;
				}
			}
			if (count == 1)
			{
				return last;
			}
			mask |= 1U<<k;
			value |= o2
				? (zeros * 2 <= count) << k
				: (zeros * 2 > count) << k;
		}
		return value;
	}

	size_t life_support() const
	{
		return filter(true) * filter(false);
	}
};

istream& operator>>(istream& in, Report& r)
{
	r.values.clear();
	r.bitlen = 0;

	string tmp;
	while (getline(in, tmp))
	{
		unsigned value = 0;
		for (size_t i = 0; i < tmp.size(); i++)
		{
			value = value << 1 | (tmp.at(i) == '1');
		}
		r.values.push_back(value);
		if (!r.bitlen)
		{
			r.bitlen = tmp.size();
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

	Report r;
	in >> r;
	in.close();
	fmt::print("Part1: {}\n", r.power_consumption());
	fmt::print("Part2: {}\n", r.life_support());
	return 0;
}
