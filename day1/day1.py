#!/usr/bin/env python3

import sys

def part1(depths):
    count = 0
    prev = depths[0]
    for cur in depths[1:]:
        if cur > prev:
            count += 1
        prev = cur
    return count

def solve(depths, l):
    if len(depths) < l:
        return 0

    count = 0
    csum = sum(depths[:l])
    for i in range(l, len(depths)):
        nsum = csum + depths[i] - depths[i-l]
        if csum < nsum:
            count += 1
        csum = nsum

    return count

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <filename>".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)

    try:
        with open(sys.argv[1], "rt") as f:
            depths = [int(x) for x in f.read().strip().split()]

        print("Part1:", solve(depths, 1))
        print("Part2:", solve(depths, 3))
    except FileNotFoundError:
        print("Cannot open {}".format(sys.argv[1]), file=sys.stderr)
        sys.exit(1)
