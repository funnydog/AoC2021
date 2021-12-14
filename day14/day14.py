#!/usr/bin/env python3

def parse(txt):
    polymer = None
    rules = {}
    state = 0
    for line in txt.splitlines():
        if state == 0:
            if line == "":
                state = 1
            else:
                polymer = line
        else:
            pattern, insertion = line.split(" -> ")
            rules[pattern] = insertion

    return polymer, rules

def solve(initial, rules, steps):
    # populate the dictionary of the couples
    couples = {}
    for i in range(len(polymer)-1):
        couple = polymer[i:i+2]
        couples[couple] = couples.get(couple, 0) + 1

    # populate the dictionary of the letter counts
    letters = {}
    for l in polymer:
        letters[l] = letters.get(l, 0) + 1

    # perform the steps
    for i in range(steps):
        ncouples = {}
        for couple, count in couples.items():
            m = rules.get(couple)
            if m is None:
                # no rule applied
                ncouples[couple] = ncouples.get(couple, 0) + count
            else:
                a = couple[0] + m
                b = m + couple[1]
                ncouples[a] = ncouples.get(a, 0) + count
                ncouples[b] = ncouples.get(b, 0) + count
                letters[m] = letters.get(m, 0) + count

        couples = ncouples

    return max(letters.values()) - min(letters.values())

import sys

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <filename>".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)

    try:
        with open(sys.argv[1], "rt") as f:
            txt = f.read()

        polymer, rules = parse(txt)

        print("Part1:", solve(polymer, rules, 10))
        print("Part2:", solve(polymer, rules, 40))
    except FileNotFoundError:
        print("Cannot open {}".format(sys.argv[1]), file=sys.stderr)
        sys.exit(1)
