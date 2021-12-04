#!/usr/bin/env python3

def parse(txt):
    return [[int(y) for y in x] for x in txt.splitlines()]

def part1(values):
    bitlength = len(values[0])
    count = len(values)
    gamma, epsilon = 0, 0
    for i in range(bitlength):
        ones = sum(x[i] for x in values)
        zeros = count - ones

        gamma <<= 1
        epsilon <<= 1
        if ones > zeros:
            gamma |= 1
        else:
            epsilon |= 1

    return gamma * epsilon

def decimate(values, criteria):
    bitlength = len(values[0])
    count = len(values)
    for i in range(bitlength):
        ones = sum(x[i] for x in values)
        zeros = count - ones

        chosen = criteria(ones, zeros)
        values = list(filter(lambda x: x[i] == chosen, values))
        count = len(values)
        if count == 1:
            break

    value = 0
    for x in values[0]:
        value <<= 1
        value |= x

    return value

def part2(values):
    def oxy(ones, zeros):
        if ones >= zeros:
            return 1
        else:
            return 0

    def co2(ones, zeros):
        if zeros <= ones:
            return 0
        else:
            return 1

    return decimate(values, oxy) * decimate(values, co2)

import sys

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <filename>".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)

    try:
        with open(sys.argv[1], "rt") as f:
            values = parse(f.read())

        print("Part1:", part1(values))
        print("Part2:", part2(values))
    except FileNotFoundError:
        print("Cannot open {}".format(sys.argv[1]), file=sys.stderr)
        sys.exit(1)

