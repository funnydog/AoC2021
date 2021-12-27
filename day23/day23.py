#!/usr/bin/env python3

import heapq
import sys

energy = {
    "A": 1,
    "B": 10,
    "C": 100,
    "D": 1000,
}

target = {
    "A": 3,
    "B": 5,
    "C": 7,
    "D": 9,
}

class Map(object):
    def __init__(self):
        self.data = None
        self.energy = 0

    def from_txt(self, txt):
        self.data = tuple(x for x in txt.splitlines())

    def swap(self, a, b):
        x0, y0 = a
        x1, y1 = b

        data = list(self.data)
        t = data[y0][x0]
        data[y0] = data[y0][:x0] + data[y1][x1] + data[y0][x0+1:]
        data[y1] = data[y1][:x1] + t + data[y1][x1+1:]

        m = Map()
        m.data = tuple(data)
        return m

    def insert_at(self, row, line):
        data = list(self.data)
        data.insert(row, line)
        self.data = tuple(data)

    def complete(self):
        for y, row in enumerate(self.data):
            for x, e in enumerate(row):
                if e in "ABCD" and x != target[e]:
                    return False
        return True

    def can_move(self, p):
        x, y = p
        if y == 1:
            return True
        if self.data[y-1][x] != ".":
            return False
        e = self.data[y][x]
        if x != target[e]:
            return True
        for yt in range(len(self.data)-1, y, -1):
            t = self.data[yt][x]
            if t == "#":
                pass
            elif t != e:
                return True
        return False

    def find_home(self, e, start):
        lst = []
        x0, y0 = start
        x1 = target[e]
        if x1 > x0:
            dx = 1
        else:
            dx = -1

        # walk horizontally
        for x in range(x0+dx, x1+dx, dx):
            if self.data[y0][x] != ".":
                return lst

        # check vertically
        for y1 in range(len(self.data)-2, 1, -1):
            t = self.data[y1][x1]
            if t == ".":
                lst.append((x1, y1))
                break
            elif t != e:
                break

        return lst

    def find_hallways(self, e, start):
        lst = []
        x0, y0 = start

        # walk up
        for y in range(y0-1, 0, -1):
            if self.data[y][x0] != ".":
                return lst

        # walk left
        for x in range(x0-1, -1, -1):
            if self.data[y][x] != ".":
                break
            elif x in (3, 5, 7, 9):
                pass
            else:
                lst.append((x, y))

        # walk right
        for x in range(x0+1, 13):
            if self.data[y][x] != ".":
                break
            elif x in (3,5,7,9):
                pass
            else:
                lst.append((x, y))

        return lst

    def __hash__(self):
        return hash(self.data)

    def __eq__(self, other):
        return self.data == other.data

    def __lt__(self, other):
        return self.data < other.data

def manhattan(a, b):
    return abs(b[0]-a[0]) + abs(b[1]-a[1])

def decisions(m):
    res = []
    for y, row in enumerate(m.data):
        for x, e in enumerate(row):
            src = (x, y)
            if e in "ABCD" and m.can_move(src):
                if y == 1:
                    dsts = m.find_home(e, src)
                else:
                    dsts = m.find_hallways(e, src)

                for dst in dsts:
                    en = manhattan(src, dst) * energy[e]
                    res.append((en, m.swap(src, dst)))

    return res

def dijkstra(start):
    visited = {}
    heap = []

    visited[start] = 0
    heapq.heappush(heap, (0, start))

    while heap:
        dist, m = heapq.heappop(heap)
        if m.complete():
            return dist

        for dd, mm in decisions(m):
            ndist = dist + dd
            if not mm in visited or ndist < visited[mm]:
                visited[mm] = ndist
                heapq.heappush(heap, (ndist, mm))

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <filename>".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)

    try:
        with open(sys.argv[1], "rt") as f:
            txt = f.read()

        m = Map()
        m.from_txt(txt)
        print("Part1:", dijkstra(m))

        m.insert_at(3,"  #D#C#B#A#")
        m.insert_at(4,"  #D#B#A#C#")
        print("Part2:", dijkstra(m))
    except FileNotFoundError:
        print("Cannot open {}".format(sys.argv[1]), file=sys.stderr)
        sys.exit(1)
