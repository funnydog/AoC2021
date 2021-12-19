#!/usr/bin/env python3

import re

class Node(object):
    PAIR = 0
    NUMBER = 1

    def __init__(self, parent, typ, number=None, left=None, right=None):
        self.parent = parent
        self.typ = typ
        self.number = number
        self.left = left
        if self.left:
            self.left.parent = self
        self.right = right
        if self.right:
            self.right.parent = self

    def __str__(self):
        if self.typ == self.PAIR:
            return "[{},{}]".format(self.left, self.right)
        else:
            return str(self.number)


PATTERN = re.compile(r"[\[\],]|\d+")
def parse(txt):
    lst = []
    for line in txt.splitlines():
        s = iter(PATTERN.findall(line))
        lst.append(parse_node(s, None))
    return lst

def parse_node(s, parent):
    value = next(s)
    if value == "[":
        n = Node(parent, Node.PAIR)
        n.left = parse_node(s, n)
        next(s)                 # ,
        n.right = parse_node(s, n)
        next(s)                 # ]
        return n
    else:
        return Node(parent, Node.NUMBER, number = int(value))

def add_left(n, val):
    while n.parent and n.parent.left == n:
        n = n.parent

    if not n.parent:
        return None

    n = n.parent.left
    while n and n.typ == n.PAIR:
        n = n.right

    if n:
        n.number += val

def add_right(n, val):
    while n.parent and n.parent.right == n:
        n = n.parent

    if not n.parent:
        return None

    n = n.parent.right
    while n and n.typ == n.PAIR:
        n = n.left

    if n:
        n.number += val

def explode(node, lvl):
    if lvl >= 4 and node.typ == node.PAIR:
        add_left(node, node.left.number)
        add_right(node, node.right.number)
        node.typ = node.NUMBER
        node.number = 0
        node.left = node.right = None
    elif node.typ == node.PAIR:
        explode(node.left, lvl+1)
        explode(node.right, lvl+1)
    else:
        pass

action = False
def split(node, lvl):
    global action
    if action:
        pass
    elif node.typ == node.PAIR:
        split(node.left, lvl+1)
        split(node.right, lvl+1)
    elif node.number >= 10:
        action = True
        node.typ = node.PAIR
        node.left = Node(node, node.NUMBER, node.number // 2)
        node.right = Node(node, node.NUMBER, (node.number + 1) // 2)
        explode(node, lvl)
    else:
        pass

def red(tree):
    global action
    explode(tree, 0)
    while True:
        action = False
        split(tree, 0)
        if not action:
            break

    return tree

def clone(a, parent):
    if a.typ == Node.NUMBER:
        return Node(parent, Node.NUMBER, number = a.number)
    else:
        n = Node(parent, Node.PAIR)
        n.left = clone(a.left, n)
        n.right = clone(a.right, n)
        return n

def add(a, b):
    n = Node(None, Node.PAIR)
    n.left = clone(a, n)
    n.right = clone(b, n)
    return red(n)

def magnitude(n):
    if n.typ == n.NUMBER:
        return n.number
    else:
        return 3 * magnitude(n.left) + 2 * magnitude(n.right)

def part1(numbers):
    a = numbers[0]
    for b in numbers[1:]:
        a = add(a, b)
    return magnitude(a)

def part2(numbers):
    maxsum = 0
    for i, a in enumerate(numbers):
        for j, b in enumerate(numbers):
            if i == j:
                continue

            m = magnitude(add(a, b))
            if maxsum < m:
                maxsum = m

    return maxsum

import sys

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <filename>".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)

    try:
        with open(sys.argv[1], "rt") as f:
            txt = f.read()

        numbers = parse(txt)
        print("Part1:", part1(numbers))
        print("Part2:", part2(numbers))
    except FileNotFoundError:
        print("Cannot open {}".format(sys.argv[1]), file=sys.stderr)
        sys.exit(1)
