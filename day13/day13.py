#!/usr/bin/env python3

class Sheet(object):
    def __init__(self, values):
        self.values = values
        self.width = max(x for x, y in values) + 1
        self.height = max(y for x, y in values) + 1

    def fold_up(self, fy):
        self.height = fy
        lst = []
        for x, y in self.values:
            if y > fy:
                lst.append((x, 2*fy-y))
            else:
                lst.append((x, y))
        self.values = lst

    def fold_left(self, fx):
        self.width = fx
        lst = []
        for x, y in self.values:
            if x > fx:
                lst.append((2*fx-x, y))
            else:
                lst.append((x, y))
        self.values = lst

    def count(self):
        return len(set(self.values))

    def __str__(self):
        s = set(self.values)
        return "\n".join(
            "".join((x, y) in s and "#" or " " for x in range(self.width))
            for y in range(self.height)
        )

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
