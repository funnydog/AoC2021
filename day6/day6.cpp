#include <cstdint>
#include <fstream>
#include <vector>

#include <fmt/format.h>

using namespace std;

static uint64_t solve(const vector<uint64_t>& ages, size_t days)
{
	vector<uint64_t> day(days+9, 0);
	for (auto f : ages)
	{
		day[f] += 1;
	}

	uint64_t sum = ages.size();
	for (size_t i = 0; i < days; i++)
	{
		sum += day[i];
		day[i + 7] += day[i];
		day[i + 9] += day[i];
	}

	return sum;
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

	vector<uint64_t> ages;
	uint64_t tmp;
	while (in>>tmp)
	{
		ages.push_back(tmp);
		in.get();
	}
	in.close();

	fmt::print("Part1: {}\n", solve(ages, 80));
	fmt::print("Part2: {}\n", solve(ages, 256));

	return 0;
}
