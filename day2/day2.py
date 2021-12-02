#!/usr/bin/env python3

def part1(program):
    x, depth = 0, 0

    for cmd, arg in program:
        if cmd == "forward":
            x += arg
        elif cmd == "down":
            depth += arg
        elif cmd == "up":
            depth -= arg

    return x * depth

def part2(program):
    x, depth, aim = 0, 0, 0

    for cmd, arg in program:
        if cmd == "forward":
            x += arg
            depth += arg * aim
        elif cmd == "down":
            aim += arg
        elif cmd == "up":
            aim -= arg

    return x * depth

import sys

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <filename>".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)

    try:
        with open(sys.argv[1], "rt") as f:
            program = [(x[0], int(x[1])) for x in (x.split() for x in f.read().splitlines())]

        print("Part1:", part1(program))
        print("Part2:", part2(program))
    except:
        print("Cannot open {}".format(sys.argv[1]), file=sys.stderr)
        sys.exit(1)
