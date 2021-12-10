#!/usr/bin/env python3

OPENED = "([{<"
CLOSED = ")]}>"
POINTS = [3, 57, 1197, 25137]

def check_line(line):
    stack = []
    for c in line:
        idx = OPENED.find(c)
        if idx >= 0:
            stack.append(idx)
        else:
            e = CLOSED[stack.pop()]
            if e != c:
                return stack, POINTS[CLOSED.index(c)]

    return stack, 0

def solve(lines):
    s = 0
    lst = []
    for line in lines:
        stack, points = check_line(line)
        s += points
        if points == 0:
            score = 0
            for i in reversed(stack):
                score = score * 5 + i + 1
            lst.append(score)

    lst = sorted(lst)
    return s, lst[len(lst)//2]

import sys

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <filename>".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)

    try:
        with open(sys.argv[1], "rt") as f:
            txt = f.read()

        part1, part2 = solve(txt.splitlines())
        print("Part1:", part1)
        print("Part2:", part2)

    except FileNotFoundError:
        print("Cannot open {}".format(sys.argv[1]), file=sys.stderr)
        sys.exit(1)
