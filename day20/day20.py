#!/usr/bin/env python3

class Map(object):
    def __init__(self, m, bg):
        self.m = m
        self.swap = {}
        self.bg = bg
        xmin = ymin = 1E10
        xmax = ymax = -1E10
        for x, y in self.m.keys():
            if xmin > x: xmin = x
            if xmax < x: xmax = x
            if ymin > y: ymin = y
            if ymax < y: ymax = y
        self.pmin = xmin, ymin
        self.pmax = xmax+1, ymax+1

    def enhance(self, index):
        if self.bg == "#":
            nbg = index[-1]
        else:
            nbg = index[0]

        xmin = ymin = 1E10
        xmax = ymax = -1E10
        self.swap.clear()
        for y in range(self.pmin[1]-2, self.pmax[1]+2):
            for x in range(self.pmin[0]-2, self.pmax[0]+2):
                idx = 0
                for ny in range(y-1, y+2):
                    for nx in range(x-1, x+2):
                        idx <<= 1
                        if self.m.get((nx, ny), self.bg) == "#":
                            idx |= 1
                if index[idx] != nbg:
                    self.swap[x, y] = index[idx]
                    if xmin > x: xmin = x
                    if xmax < x: xmax = x
                    if ymin > y: ymin = y
                    if ymax < y: ymax = y

        self.m, self.swap = self.swap, self.m
        self.bg = nbg
        self.pmin = xmin, ymin
        self.pmax = xmax+1, ymax+1

def parse(txt):
    algo, m = None, {}
    state = 0
    y = 0
    for row in txt.splitlines():
        if state == 0:
            if not row:
                state = 1
            else:
                algo = row
        else:
            for x, e in enumerate(row):
                if e == "#":
                    m[x, y] = e
            y += 1

    return Map(m, "."), algo

import sys

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <filename>".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)

    try:
        with open(sys.argv[1], "rt") as f:
            txt = f.read()

        m, algo = parse(txt)
        for i in range(2):
            m.enhance(algo)
        print("Part1:", len(m.m))
        for i in range(48):
            m.enhance(algo)
        print("Part2:", len(m.m))
    except FileNotFoundError:
        print("Cannot open {}".format(sys.argv[1]), file=sys.stderr)
        sys.exit(1)
