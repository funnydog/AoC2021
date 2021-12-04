#!/usr/bin/env python3

class Board(object):
    def __init__(self, numbers):
        assert len(numbers) == 25, "Boards have exactly 25 numbers"
        self.numbers = numbers
        self.marked = [False] * 25
        self.won = False

    def mark_and_check(self, number):
        try:
            pos = self.numbers.index(number)
            self.marked[pos] = True

            # NOTE: this will short-circuit the expensive checks
            # ordered by their cost
            self.won = self.won \
                or self.check_row(pos) \
                or self.check_column(pos)

        except ValueError:
            pass

        return self.won

    def check_row(self, pos):
        start = pos - (pos%5)
        for x in range(start, start+5):
            if not self.marked[x]:
                return False

        return True

    def check_column(self, pos):
        for y in range(pos%5, 25, 5):
            if not self.marked[y]:
                return False

        return True

    def score(self, number):
        score = 0
        for i, x in enumerate(self.numbers):
            if not self.marked[i]:
                score += x

        return score * number

def parse_boards(txt):
    numbers = []
    boards = []
    board = []
    state = 0
    for line in txt.splitlines():
        if state == 0:
            # read numbers
            if line:
                numbers = list(int(x) for x in line.split(","))
            else:
                state = 1
        else:
            # read boards
            if line:
                board += list(int(x) for x in line.split())
            else:
                boards.append(Board(board))
                board = []
    if board:
        boards.append(Board(board))

    return numbers, boards

def part1(boards, numbers):
    for n in numbers:
        for b in boards:
            if b.mark_and_check(n):
                return b.score(n)
    return 0

def part2(boards, numbers):
    for n in numbers:
        for b in boards:
            b.mark_and_check(n)
        if len(boards) == 1 and boards[0].won:
            return boards[0].score(n)
        else:
            boards = list(filter(lambda b: not b.won, boards))
    return 0

import sys

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <filename>".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)

    try:
        with open(sys.argv[1], "rt") as f:
            txt = f.read()

        numbers, boards = parse_boards(txt)
        print("Part1:", part1(boards, numbers))
        print("Part2:", part2(boards, numbers))
    except FileNotFoundError:
        print("Cannot open {}".format(sys.argv[1]), file=sys.stderr)
