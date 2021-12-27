#!/usr/bin/env python3

with open("input", "rt") as f:
    txt = f.read()

class Map(object):
    def __init__(self, txt):
        self.m = [list(row) for row in txt.splitlines()]
        self.swap = None
        self.width, self.height = len(self.m[0]), len(self.m)

    def single_step(self, element, dx, dy):
        self.swap = [["." for x in range(self.width)] for y in range(self.height)]
        for y, row in enumerate(self.m):
            for x, e in enumerate(row):
                if e == ".":
                    pass
                elif e != element:
                    self.swap[y][x] = e
                else:
                    nx = (x + dx) % self.width
                    ny = (y + dy) % self.height
                    if self.m[ny][nx] == ".":
                        self.swap[ny][nx] = element
                    else:
                        self.swap[y][x] = element

        self.m = self.swap

    def step(self):
        old = self.m
        self.single_step(">", 1, 0)
        self.single_step("v", 0, 1)

        # deadlock check
        for y, row in enumerate(old):
            for x, e in enumerate(row):
                if e != self.m[y][x]:
                    return True
        return False

    def __str__(self):
        return "\n".join("".join(row) for row in self.m)

import sys

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <filename>".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)

    try:
        with open(sys.argv[1], "rt") as f:
            txt = f.read()

        m = Map(txt)
        i = 0
        while True:
            i += 1
            if not m.step():
                break

        print("Part1:", i)
    except FileNotFoundError:
        print("Cannot open {}".format(sys.argv[1]), file=sys.stderr)
        sys.exit(1)
