#!/usr/bin/env python3

def backtrack(lst, assigned, m):
    cur = lst[-1]
    if cur == "end":
        return 1

    count = 0
    for i in m.get(cur, []):
        should_skip = i.upper() != i and i in lst
        if should_skip and assigned or i == "start":
            continue

        lst.append(i)
        count += backtrack(lst, assigned or should_skip, m)
        lst.pop()

    return count

import sys

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <filename>".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)

    try:
        with open(sys.argv[1], "rt") as f:
            txt = f.read()

        m = {}
        for line in txt.splitlines():
            src, dst = line.split("-")
            m[src] = m.get(src, []) + [dst]
            m[dst] = m.get(dst, []) + [src]

        lst = ["start"]
        print("Part1:", backtrack(lst, True, m))
        print("Part2:", backtrack(lst, False, m))
    except FileNotFoundError:
        print("Cannot open {}".format(sys.argv[1]), file=sys.stderr)
        sys.exit(1)
