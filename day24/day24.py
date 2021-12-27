#!/usr/bin/env python3

import sys

def constants(frags):
    # find the lines with the differences
    lines = set()
    for i, fa in enumerate(frags):
        for j in range(0, i):
            b = frags[j].splitlines()
            for k, ia in enumerate(fa.splitlines()):
                ib = b[k]
                if ia != ib:
                    lines.add(k)

    # extract the constants
    clist = []
    for f in frags:
        lst = []
        for i, line in enumerate(f.splitlines()):
            if i in lines:
                lst.append(int(line.split()[2]))

        clist.append(tuple(lst))

    return clist

def shot(a, b, c, d, z):
    "Python equivalent of the assembly algorithm in each fragment"
    if (z % 26) + b != d:
        return (z // a) * 26 + d + c
    else:
        return z // a

solutions = []

def backtrack(res, k, z, clist):
    global solutions
    if k == 14:
        if z == 0:
            value = 0
            for d in res:
                value = value * 10 + d
            solutions.append(value)
    else:
        a, b, c = clist[k]
        for d in range(9, 0, -1):
            if b > 0 or z % 26 + b == d:
                res.append(d)
                backtrack(res, k+1, shot(a, b, c, d, z), clist)
                res.pop()

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <filename>".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)

    try:
        with open(sys.argv[1], "rt") as f:
            txt = f.read()

        # divide the program in fragments starting with inp
        # instructions.
        frags = txt.split("inp w\n")[1:]

        # extract the differences in constants for each fragment
        clist = constants(frags)

        # look for solutions
        backtrack([], 0, 0, clist)
        print("Part1:", solutions[0])
        print("Part2:", solutions[-1])
    except FileNotFoundError:
        print("Cannot open {}".format(sys.argv[1]), file=sys.stderr)
        sys.exit(1)
