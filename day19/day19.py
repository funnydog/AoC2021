#!/usr/bin/env python3

import sys
import math

class Scanner(object):
    def __init__(self, id):
        self.id = id
        self.beacons = []
        self.ref = None         # frame of reference
        self.tran = None        # inverse of translation against ref
        self.rot = None         # rotation against ref
        self.pos = None         # absolute position

    def add(self, txt):
        self.beacons.append(tuple(int(x) for x in txt.split(",")))

    def __repr__(self):
        return "<Scanner {} {} {}: {}>".format(self.id, self.tran, self.rot, self.beacons)

def parse(txt):
    state = 0
    scanner = None
    lst = []
    for row in txt.splitlines():
        if row.startswith("---"):
            scanner = Scanner(int(row.split()[2]))
            lst.append(scanner)
        elif row == "":
            pass
        else:
            scanner.add(row)
    return lst

def mmul(a, b):
    lst = []
    for y in range(3):
        row = []
        for x in range(3):
            row.append(sum(a[y][i] * b[i][x] for i in range(3)))
        lst.append(tuple(row))
    return tuple(lst)

def vmul(m, v):
    return tuple(sum(m[y][x] * v[x] for x in range(3)) for y in range(3))

def sub(a, b):
    return tuple(a[i]-b[i] for i in range(3))

def manhattan(a, b):
    return sum(abs(a[i]-b[i]) for i in range(3))

def rotations():
    m = set()
    for alfa in range(4):
        rc = int(math.cos(math.pi * 0.5 * alfa))
        rs = int(math.sin(math.pi * 0.5 * alfa))
        rm = ((1, 0, 0), (0, rc, -rs), (0, rs, rc))
        for beta in range(4):
            pc = int(math.cos(math.pi * 0.5 * beta))
            ps = int(math.sin(math.pi * 0.5 * beta))
            pm = ((pc, 0, ps), (0, 1, 0), (-ps, 0, pc))
            for gamma in range(4):
                yc = int(math.cos(math.pi * 0.5 * gamma))
                ys = int(math.sin(math.pi * 0.5 * gamma))
                ym = ((yc, -ys, 0), (ys, yc, 0), (0, 0, 1))
                m.add(mmul(mmul(rm, pm), ym))

    return tuple(m)

rots = rotations()

def set_reference(ref, s):
    dist = {}
    for r in rots:
        dist.clear()
        for b in s.beacons:
            br = vmul(r, b)
            for p in ref.beacons:
                delta = sub(br, p)
                v = dist[delta] = dist.get(delta, 0) + 1
                if v >= 12:
                    s.ref = ref
                    s.tran = delta
                    s.rot = r
                    return True

    return False

def update_map(m, s):
    for b in s.beacons:
        t = s
        while t.ref:
            b = sub(vmul(t.rot, b), t.tran)
            t = t.ref
        m.add(b)

def find_position(s):
    point = (0, 0, 0)
    sc = s
    while sc.ref:
        point = sub(vmul(sc.rot, point), sc.tran)
        sc = sc.ref
    s.pos = point

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <filename>".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)

    try:
        with open(sys.argv[1], "rt") as f:
            txt = f.read()

        scanners = parse(txt)

        solved = 1
        i = 0
        while i < solved:
            ref = scanners[i]
            j = solved
            while j < len(scanners):
                s = scanners[j]
                if set_reference(ref, s):
                    scanners[solved], scanners[j] = scanners[j], scanners[solved]
                    solved += 1
                j += 1
            i += 1

        beacons = set()
        for s in scanners:
            update_map(beacons, s)

        print("Part1:", len(beacons))

        for s in scanners:
            find_position(s)

        maxd = 0
        for a in scanners:
            for b in scanners:
                if a.id != b.id:
                    d = manhattan(a.pos, b.pos)
                    if maxd < d:
                        maxd = d
        print("Part2:", maxd)
    except FileNotFoundError:
        print("Cannot open {}".format(sys.argv[1]), file=sys.stderr)
        sys.exit(1)
