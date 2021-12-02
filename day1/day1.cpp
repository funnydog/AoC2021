#include <fstream>
#include <vector>

#include <fmt/format.h>

using namespace std;

static size_t solve(const vector<int>& depths, size_t l)
{
	size_t count = 0;
	if (depths.size() >= l)
	{
		int sum = 0;
		for (size_t i = 0; i < l; i++)
		{
			sum += depths[i];
		}
		for (size_t i = l; i < depths.size(); i++)
		{
			int nsum = sum + depths[i] - depths[i-l];
			if (nsum > sum)
			{
				count++;
			}
			sum = nsum;
		}
	}
	return count;
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
		fmt::print(stderr, "Cannot open {}", argv[1]);
		return 1;
	}

	vector<int> depths;
	int depth;
	while (in >> depth)
	{
		depths.push_back(depth);
	}
	in.close();

	fmt::print("Part1: {}\n", solve(depths, 1));
	fmt::print("Part2: {}\n", solve(depths, 3));

	return 0;
}
