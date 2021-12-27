#!/usr/bin/env python3

def part1(p1, p2):
    s1, s2 = 0, 0
    dice = 1
    count = 0
    while True:
        value = 0
        for i in range(3):
            count += 1
            if dice > 100:
                dice = 1
            value += dice
            dice += 1

        p1 = (p1 + value - 1) % 10 + 1
        s1 += p1
        if s1 >= 1000:
            break
        p1, p2 = p2, p1
        s1, s2 = s2, s1
    return s2 * count

from functools import cache

dice = [i+j+k for i in (1,2,3) for j in (1,2,3) for k in (1,2,3)]

@cache
def find(p1, p2, s1, s2):
    w1, w2 = 0, 0
    for r in dice:
        np1 = (p1 + r - 1) % 10 + 1
        ns1 = s1 + np1
        if ns1 >= 21:
            w1 += 1
        else:
            t2, t1 = find(p2, np1, s2, ns1)
            w1 += t1
            w2 += t2

    return w1, w2

def part2(p1, p2):
    return max(find(p1, p2, 0, 0))

def parse(txt):
    return tuple(int(x.split()[4]) for x in txt.splitlines())

import sys

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <filename>".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)

    try:
        with open(sys.argv[1], "rt") as f:
            txt = f.read()

        p1, p2 = parse(txt)
        print("Part1:", part1(p1, p2))
        print("Part2:", part2(p1, p2))
    except FileNotFoundError:
        print("Cannot open {}".format(sys.argv[1]), file=sys.stderr)
        sys.exit(1)
