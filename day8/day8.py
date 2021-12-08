#!/usr/bin/env python3

letters = "abcdefg"
segments = [
    "abcefg",
    "cf",
    "acdeg",
    "acdfg",
    "bcdf",
    "abdfg",
    "abdefg",
    "acf",
    "abcdefg",
    "abcdfg",
]

def encode(lseg):
    nseg = 0
    for l in lseg:
        nseg |= 1 << letters.index(l)
    return nseg

def count_ones(i):
    return bin(i).count("1")

# common segments between digits
intersections = [[count_ones(encode(segments[i]) & encode(segments[j]))
                  for i in range(10)]
                 for j in range(10)]

# map a segment length to candidates numbers
candidates = {}
for i, seg in enumerate(segments):
    l = len(seg)
    candidates[l] = candidates.get(l, []) + [i]

def part1(values):
    count = 0
    for _, display in values:
        for value in display:
            if len(candidates[count_ones(value)]) == 1:
                count += 1
    return count

def solve(row):
    wiring, display = row

    # find the candidates for each digit
    nums = {}
    for wire in wiring:
        for i in candidates[count_ones(wire)]:
            nums[i] = nums.get(i, []) + [wire]

    # solve the constraints
    order = sorted(range(10), key = lambda x: len(nums[x]))
    t = 0
    while len(nums[order[t]])==1:
        t += 1

    k = 0
    while k < t:
        i = order[k]
        iseg = nums[i][0]
        for l in range(t, 10):
            j = order[l]
            nums[j] = [jseg for jseg in nums[j] if count_ones(iseg & jseg) == intersections[i][j]]
            if len(nums[j]) == 1:
                order[t], order[l] = order[l], order[t]
                t += 1
        k += 1

    if t < 10:
        print("Couldn't find a solution")
        return 0

    # map the encoded digit to the the actual value
    segmap = {v[0]: k for k, v in nums.items()}

    # compute the value in the display
    value = 0
    for digit in display:
        value = value * 10 + segmap[digit]

    return value

def part2(values):
    return sum(solve(x) for x in values)

import sys

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <filename>".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)

    try:
        with open(sys.argv[1], "rt") as f:
            txt = f.read()

        values = []
        for row in txt.strip().splitlines():
            wiring, display = row.split(" | ")
            values.append(
                ([encode(x) for x in wiring.split()],
                 [encode(x) for x in display.split()])
            )

        print("Part1:", part1(values))
        print("Part2:", part2(values))
    except FileNotFoundError:
        print("Cannot open {}".format(sys.argv[1]), file=sys.stderr)
