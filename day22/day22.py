#!/usr/bin/env python3

class Cuboid(object):
    def __init__(self, p0, p1):
        self.p0 = p0
        self.p1 = p1

    def inside(self, other):
        for i in range(3):
            if self.p0[i] < other.p0[i] or self.p1[i] > other.p1[i]:
                return False
        return True

    def intersect(self, other):
        p0 = []
        p1 = []
        for i in range(3):
            v0 = max(self.p0[i], other.p0[i])
            v1 = min(self.p1[i], other.p1[i])
            if v0 >= v1:
                return None
            p0.append(v0)
            p1.append(v1)
        return Cuboid(tuple(p0), tuple(p1))

    def volume(self):
        v = 1
        for i in range(3):
            v *= self.p1[i] - self.p0[i]
        return v

def parse_instructions(txt):
    lst = []
    for row in txt.splitlines():
        column = row.split()
        instr = column[0]
        coords = column[1].split(",")
        coords = [tuple(int(v) for v in x[2:].split("..")) for x in coords]

        (x0, x1), (y0, y1), (z0, z1) = coords
        lst.append((instr, Cuboid((x0, y0, z0), (x1+1, y1+1, z1+1))))
    return lst

def volume(lst):
    return sum(c.volume() for c in lst)

def sub(on, off, cuboid):
    end = len(on)
    for c in off:
        v = c.intersect(cuboid)
        if v:
            on.append(v)

    for c in on[:end]:
        v = c.intersect(cuboid)
        if v:
            off.append(v)

def add(on, off, cuboid):
    sub(on, off, cuboid)
    on.append(cuboid)

import sys

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <filename>".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)

    try:
        with open(sys.argv[1], "rt") as f:
            txt = f.read()

        part1 = 0
        region = Cuboid((-50,-50,-50), (51, 51, 51))
        on, off = [], []
        for ins, cuboid in parse_instructions(txt):

            if not part1 and not cuboid.inside(region):
                part1 = volume(on) - volume(off)

            if ins == "on":
                add(on, off, cuboid)
            elif ins == "off":
                sub(on, off, cuboid)

        print("Part1:", part1)
        print("Part2:", volume(on) - volume(off))
    except FileNotFoundError:
        print("Cannot open {}".format(sys.argv[1]), file=sys.stderr)
        sys.exit(1)
