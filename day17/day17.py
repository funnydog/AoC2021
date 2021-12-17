#!/usr/bin/env python3

import math

def parse_area(txt):
    _, txt = txt.split(":")
    tx, ty = txt.split(",")
    x0, x1 = (int(x) for x in tx[3:].split(".."))
    y0, y1 = (int(y) for y in ty[3:].split(".."))
    return x0, x1, y0, y1

def sim(vx, vy, x0, x1, y0, y1):
    x, y = 0, 0
    if vy >= 0:
        # shortcut
        t = 2 * vy + 1
        vy = -vy - 1
        x = (vx + 1) * vx // 2
        if vx > t:
            vx -= t
        else:
            vx = 0
        x -= (vx + 1) * vx // 2

    while True:
        x += vx
        y += vy
        if vx:
            vx -= 1
        vy -= 1

        if x > x1 or y < y0:
            return False
        if x0 <= x and y <= y1:
            return True

def simx(vx, x0, x1):
    x = 0
    while True:
        x = x + vx
        if vx:
            vx -= 1

        if x > x1:
            return False
        elif x0 <= x:
            return True
        elif vx == 0:
            return False
        else:
            pass

def find_vx(x0, x1):
    lst = []
    for vx in range(math.ceil((math.sqrt(1 + 8 * x0)-1)/2), x1+1):
        if simx(vx, x0, x1):
            lst.append(vx)
    return lst

import sys

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <filename>".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)

    try:
        with open(sys.argv[1], "rt") as f:
            txt = f.read()

        x0, x1, y0, y1 = parse_area(txt)

        vy_max = (y0+1) * (y0) // 2
        vy_min = y0

        count = 0
        for vx in find_vx(x0, x1):
            for vy in range(vy_min, vy_max + 1):
                if sim(vx, vy, x0, x1, y0, y1):
                    count += 1

        print("Part1:", vy_max)
        print("Part2:", count)
    except FileNotFoundError:
        print("Cannot open {}".format(sys.argv[1]), file=sys.stderr)
        sys.exit(1)
