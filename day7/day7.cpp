#include <algorithm>
#include <limits>
#include <fstream>
#include <functional>
#include <numeric>
#include <vector>

#include <fmt/format.h>

using namespace std;

static int solve(const vector<int>& crabs, int min, int max, const std::function<int(int)> &cost)
{
	int minfuel = numeric_limits<int>::max();
	for (int p = min; p <= max; p++)
	{
		int fuel = accumulate(
			crabs.cbegin(), crabs.cend(), 0,
			[p, &cost](int acc, int x) {
				return acc + cost(abs(x-p));
			});

		if (minfuel > fuel)
		{
			minfuel = fuel;
		}
	}
	return minfuel;
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

	int tmp;
	vector<int> crabs;
	while (in>>tmp)
	{
		crabs.push_back(tmp);
		in.get();
	}
	in.close();

	auto [min, max] = minmax_element(crabs.cbegin(), crabs.cend());
	fmt::print("Part1: {}\n", solve(crabs, *min, *max, [](int d) { return d; }));
	fmt::print("Part2: {}\n", solve(crabs, *min, *max, [](int d) { return d*(d+1) / 2; }));
	return 0;
}
