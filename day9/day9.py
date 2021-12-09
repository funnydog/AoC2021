#!/usr/bin/env python3

class Map(object):
    NEIGHBORS = ((-1, 0), (0, -1), (1, 0), (0, 1))

    def __init__(self, txt):
        self.m = [[int(x) for x in row] for row in txt.splitlines()]
        self.height = len(self.m)
        self.width = len(self.m[0])

    def risk(self, x, y):
        value = self.m[y][x]
        for dx, dy in self.NEIGHBORS:
            nx = x + dx
            ny = y + dy
            if 0 <= nx < self.width \
               and 0 <= ny < self.height:
                if self.m[ny][nx] <= value:
                    return 0
        return value + 1

    def dfs_reset(self):
        self.visited = [[False for x in range(self.width)]
                        for y in range(self.height)]

    def dfs(self, x, y):
        if self.m[y][x] == 9:
            return 0
        if self.visited[y][x]:
            return 0
        self.visited[y][x] = True
        size = 1
        for dx, dy in self.NEIGHBORS:
            if 0 <= x + dx < self.width \
               and 0 <= y + dy < self.height:
                size += self.dfs(x+dx, y+dy)
        return size

def part1(m):
    return sum(m.risk(x, y) for x in range(m.width) for y in range(m.height))

def part2(m):
    basins = []
    m.dfs_reset()
    for y in range(m.height):
        for x in range(m.width):
            size = m.dfs(x, y)
            if size:
                basins.append(size)
    mul = 1
    for x in sorted(basins, key=lambda x: -x)[:3]:
        mul *= x
    return mul

import sys

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <filename>".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)

    try:
        with open(sys.argv[1], "rt") as f:
            m = Map(f.read())

        print("Part1:", part1(m))
        print("Part2:", part2(m))
    except FileNotFoundError:
        print("Cannot open {}".format(sys.argv[1]), file=sys.stderr)
        sys.exit(1)
