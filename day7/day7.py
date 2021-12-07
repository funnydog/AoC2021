#!/usr/bin/env python3

def cost1(d):
    return d

def cost2(d):
    return d * (d + 1) // 2

def solve(crabs, pmin, pmax, costfn):
    return min(sum(costfn(abs(x-p)) for x in crabs) for p in range(pmin, pmax+1))

import sys

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <filename>".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)

    try:
        with open(sys.argv[1], "rt") as f:
            crabs = list(int(x) for x in f.read().split(","))

        cmin = min(crabs)
        cmax = max(crabs)
        print("Part1:", solve(crabs, cmin, cmax, cost1))
        print("Part2:", solve(crabs, cmin, cmax, cost2))
    except FileNotFoundError:
        print("Cannot open {}".format(sys.argv[1]), file=sys.stderr)
