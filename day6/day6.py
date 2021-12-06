#!/usr/bin/env python3

def solve(ages, days):
    day = [0] * (days + 9)

    for age in ages:
        day[age] += 1

    count = len(ages)
    for i in range(days):
        count += day[i]
        day[i + 7] += day[i]
        day[i + 9] += day[i]

    return count

import sys

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <filename>".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)

    try:
        with open(sys.argv[1], "rt") as f:
            ages = list(int(x) for x in f.read().split(","))

        print("Part1:", solve(ages, 80))
        print("Part2:", solve(ages, 256))
    except FileNotFoundError:
        print("Cannot open {}".format(sys.argv[1]), file=sys.stderr)
        sys.exit(1)
