#!/usr/bin/env python3

class Sheet(object):
    def __init__(self, values):
        self.values = values

    def fold_up(self, fy):
        for i, (x, y) in enumerate(self.values):
            if y > fy:
                self.values[i] = (x, 2 * fy - y)

    def fold_left(self, fx):
        for i, (x, y) in enumerate(self.values):
            if x > fx:
                self.values[i] = (2 * fx - x, y)

    def count(self):
        return len(set(self.values))

    def __str__(self):
        lst = []

        cx, cy = 0, 0
        for x, y in sorted(self.values, key=lambda p: (p[1], p[0])):
            # NOTE: take care of duplicates
            if cy == y and cx > x:
                continue

            if y - cy:
                lst.append("\n" * (y - cy))
                cx = 0

            if x - cx:
                lst.append(" " * (x - cx))

            lst.append("#")

            cy = y
            cx = x + 1

        return "".join(lst)

def parse_sheet(txt):
    coords = []
    instr = []
    state = 0
    for line in txt.splitlines():
        if state == 0:
            if line == "":
                state = 1
            else:
                coords.append(tuple(int(x) for x in line.split(",")))
        else:
            a, b = line.split("=")
            instr.append((a[-1], int(b)))

    return Sheet(coords), instr

def solve(txt):
    s, instr = parse_sheet(txt)
    part1 = None
    for coord, pos in instr:
        if coord == "y":
            s.fold_up(pos)
        elif coord == "x":
            s.fold_left(pos)

        if part1 is None:
            part1 = s.count()

    return part1, s

import sys

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <filename>".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)

    try:
        with open(sys.argv[1], "rt") as f:
            txt = f.read()

        part1, part2 = solve(txt)
        print("Part1:", part1)
        print("Part2:")
        print(part2)
    except FileNotFoundError:
        print("Cannot open {}".format(sys.argv[1]), file=sys.stderr)
        sys.exit(1)
