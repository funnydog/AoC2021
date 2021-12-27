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
        for i in range(3):
            if self.p1[i] <= other.p0[i] or self.p0[i] >= other.p1[i]:
                return False
        return True

    def volume(self):
        v = 1
        for i in range(3):
            v *= self.p1[i] - self.p0[i]
        return v

def parse_instruction(row):
    column = row.split()
    instr = column[0]
    coords = column[1].split(",")
    coords = [tuple(int(v) for v in x[2:].split("..")) for x in coords]

    (x0, x1), (y0, y1), (z0, z1) = coords
    return instr, Cuboid((x0, y0, z0), (x1+1, y1+1, z1+1))

def replace(p1, p2, i):
    p1 = list(p1)
    p1[i] = p2[i]
    return tuple(p1)

def split(c, b):
    r = []
    q = [c]
    for i in range(3):
        for c in q:
            if c.intersect(b) and c.p0[i] <= b.p0[i] < c.p1[i]:
                r.append(Cuboid(c.p0, replace(c.p1, b.p0, i)))
                r.append(Cuboid(replace(c.p0, b.p0, i), c.p1))
            else:
                r.append(c)
        q, r = r, []

        for c in q:
            if c.intersect(b) and c.p0[i] <= b.p1[i] < c.p1[i]:
                r.append(Cuboid(c.p0, replace(c.p1, b.p1, i)))
                r.append(Cuboid(replace(c.p0, b.p1, i), c.p1))
            else:
                r.append(c)
        q, r = r, []

    return q

def sub(lst, b):
    r = []
    for c in lst:
        if not c.intersect(b):
            r.append(c)
        else:
            for a in split(c, b):
                if not a.inside(b):
                    r.append(a)
    return r

def add(lst, b):
    lst = sub(lst, b)
    lst.append(b)
    return lst

def volume(lst):
    return sum(c.volume() for c in lst)

import sys

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <filename>".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)

    try:
        with open(sys.argv[1], "rt") as f:
            txt = f.read()

        region = Cuboid((-50, -50, -50), (51, 51, 51))
        part1 = 0
        lst = []
        for ins, cuboid in (parse_instruction(row) for row in txt.splitlines()):
            if not part1 and not cuboid.inside(region):
                part1 = volume(lst)
            if ins == "on":
                lst = add(lst, cuboid)
            elif ins == "off":
                lst = sub(lst, cuboid)

        print("Part1:", part1)
        print("Part2:", volume(lst))
    except FileNotFoundError:
        print("Cannot open {}".format(sys.argv[1]), file=sys.stderr)
        sys.exit(1)
