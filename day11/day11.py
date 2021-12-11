#!/usr/bin/env python3

class Map(object):
    NEIGHBORS = ((-1, 0), (-1, -1), (0, -1),
                 (1, -1),           (1,  0),
                 (1,  1), (0,   1), (-1, 1))

    def __init__(self, m):
        self.data = [[int(x) for x in row] for row in m.splitlines()]
        self.height = len(self.data)
        self.width = len(self.data[0])

    def inc(self, x, y):
        if self.data[y][x] < 0:
            return

        self.data[y][x] += 1
        if self.data[y][x] > 9:
            self.data[y][x] = -1
            for dx, dy in self.NEIGHBORS:
                nx = x + dx
                ny = y + dy
                if 0 <= nx < self.width and 0 <= ny < self.height:
                    self.inc(nx, ny)

    def step(self):
        # increase the level
        for y in range(self.height):
            for x in range(self.width):
                self.inc(x, y)

        # zero the flashed octopuses
        # NOTE: this is why they synchronize
        count = 0
        for y in range(self.height):
            for x in range(self.width):
                if self.data[y][x] < 0:
                    count += 1
                    self.data[y][x] = 0

        return count

def solve(txt):
    m = Map(txt)
    i = 0
    part1 = 0
    while True:
        flashes = m.step()
        if i < 100:
            part1 += flashes
        if flashes == m.width * m.height:
            break
        i += 1
    return part1, i+1

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
        print("Part2:", part2)
    except FileNotFoundError:
        print("Cannot open {}".format(sys.argv[1]), file=sys.stderr)
        sys.exit(1)
