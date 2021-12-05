#!/usr/bin/env python3

def draw(m, start, end):
    x0, y0 = start
    x1, y1 = end
    sx = (x0 < x1) and 1 or (x0 > x1) and -1 or 0
    sy = (y0 < y1) and 1 or (y0 > y1) and -1 or 0

    while True:
        m[x0, y0] = m.get((x0, y0), 0) + 1
        if x0 == x1 and y0 == y1:
            break
        x0 += sx
        y0 += sy

def part1(segments):
    m = {}
    for s, e in segments:
        if s[0] == e[0] or s[1] == e[1]:
            draw(m, s, e)
    return sum(1 for x in m.values() if x > 1)

def part2(segments):
    m = {}
    for s, e in segments:
        draw(m, s, e)
    return sum(1 for x in m.values() if x > 1)

import sys

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <filename>".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)

    try:
        with open(sys.argv[1], "rt") as f:
            txt = f.read()

        segments = []
        for line in txt.splitlines():
            s0, s1 = line.split("->")
            s0 = tuple(int(x) for x in s0.split(","))
            s1 = tuple(int(x) for x in s1.split(","))
            segments.append((s0, s1))

        print("Part1:", part1(segments))
        print("Part2:", part2(segments))
    except FileNotFoundError:
        print("Cannot open {}".format(sys.argv[1]), file=sys.stderr)
