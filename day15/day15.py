#!/usr/bin/env python3

import heapq

class Map(object):
    NEIGH = ((-1,0), (0,-1), (1,0), (0, 1))

    def __init__(self, txt):
        self.m = [[int(x) for x in row] for row in txt.splitlines()]
        self.height = len(self.m)
        self.width = len(self.m[0])

    def extend(self, n):
        m = []
        for y in range(self.height * n):
            row = []
            ry = y % self.height
            qy = y // self.height
            for x in range(self.width * n):
                rx = x % self.width
                qx = x // self.width

                row.append((self.m[ry][rx] + qx + qy - 1) % 9 + 1)

            m.append(row)

        self.m = m
        self.height *= n
        self.width *= n

    def dijkstra(self, start):
        dist = [[0 for x in row] for row in self.m]
        queue = []
        heapq.heappush(queue, (0, start))
        while queue:
            risk, pos = heapq.heappop(queue)
            for dx, dy in self.NEIGH:
                nx, ny = npos = pos[0] + dx, pos[1] + dy
                if 0 <= nx < self.width and 0 <= ny < self.height:
                    nrisk = risk + self.m[ny][nx]
                    if not dist[ny][nx] or nrisk < dist[ny][nx]:
                        dist[ny][nx] = nrisk
                        heapq.heappush(queue, (nrisk, npos))

        return dist[-1][-1]

import sys

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <filename>".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)

    try:
        with open(sys.argv[1], "rt") as f:
            txt = f.read()

        m = Map(txt)
        print("Part1:", m.dijkstra((0, 0)))
        m.extend(5)
        print("Part2:", m.dijkstra((0, 0)))
    except FileNotFoundError:
        print("Cannot open {}".format(sys.argv[1]), file=sys.stderr)
        sys.exit(1)
