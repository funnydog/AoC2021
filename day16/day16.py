#!/usr/bin/env python3

HEXDIGITS = "0123456789ABCDEF"

class Bitstream(object):
    def __init__(self, hexstr):
        self.hexstr = hexstr
        self.hexpos = 0
        self.rawdata = 0
        self.avail = 0
        self.pos = 0

    def get(self, bits):
        while self.avail < bits:
            if self.hexpos >= len(self.hexstr):
                raise RuntimeError("EOF")

            hexval = HEXDIGITS.find(self.hexstr[self.hexpos])
            if hexval < 0:
                continue

            self.rawdata = self.rawdata << 4 | hexval
            self.hexpos += 1
            self.avail += 4

        self.avail -= bits
        self.pos += bits

        value = self.rawdata >> self.avail
        self.rawdata &= (1<<self.avail) - 1
        return value

def parse(stream):
    ver = stream.get(3)
    typ = stream.get(3)
    if typ == 4:
        value = 0
        while True:
            group = stream.get(5)
            value = value << 4 | (group & 15)
            if group & 16 == 0:
                break

        return (ver, typ, value)
    elif stream.get(1) == 0:
        end = stream.get(15)
        end += stream.pos
        lst = []
        while stream.pos < end:
            lst.append(parse(stream))
        return (ver, typ, tuple(lst))
    else:
        count = stream.get(11)
        lst = tuple(parse(stream) for x in range(count))
        return (ver, typ, lst)

def interp1(tree):
    ver, typ, children = tree
    if typ == 4:
        return ver
    else:
        return ver + sum(interp1(x) for x in children)

def interp2(tree):
    ver, typ, children = tree
    if typ == 0:                # sum
        return sum(interp2(x) for x in children)
    elif typ == 1:              # product
        m = 1
        for x in children:
            m *= interp2(x)
        return m
    elif typ == 2:              # minimum
        return min(interp2(x) for x in children)
    elif typ == 3:              # maximum
        return max(interp2(x) for x in children)
    elif typ == 4:              # value
        return children
    elif typ == 5:              # greater than
        a = interp2(children[0])
        b = interp2(children[1])
        if a > b:
            return 1
        else:
            return 0
    elif typ == 6:              # less than
        a = interp2(children[0])
        b = interp2(children[1])
        if a < b:
            return 1
        else:
            return 0
    elif typ == 7:              # equal to
        a = interp2(children[0])
        b = interp2(children[1])
        if a == b:
            return 1
        else:
            return 0
    else:
        raise RuntimeError("unknown type")

def tolisp(tree):
    var, typ, children = tree
    if typ == 0:                # sum
        return "(+ " + " ".join(tolisp(x) for x in children) + ")"
    elif typ == 1:              # product
        return "(* " + " ".join(tolisp(x) for x in children) + ")"
    elif typ == 2:              # minimum
        return "(min " + " ".join(tolisp(x) for x in children) + ")"
    elif typ == 3:              # maximum
        return "(max " + " ".join(tolisp(x) for x in children) + ")"
    elif typ == 4:              # value
        return str(children)
    elif typ == 5:              # greater than
        return "(if (> " + " ".join(tolisp(x) for x in children) + ") 1 0)"
    elif typ == 6:              # less than
        return "(if (< " + " ".join(tolisp(x) for x in children) + ") 1 0)"
    elif typ == 7:              # equal to
        return "(if (= " + " ".join(tolisp(x) for x in children) + ") 1 0)"
    else:
        raise RuntimeError("unknown type")

import sys

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <filename>".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)

    try:
        with open(sys.argv[1], "rt") as f:
            txt = f.read()

        for line in txt.splitlines():
            program = parse(Bitstream(line))
            print("Part1:", interp1(program))
            print("Part2:", interp2(program))
            #print("Lisp:", tolisp(program))
    except FileNotFoundError:
        print("Cannot open {}".format(sys.argv[1]), file=sys.stderr)
        sys.exit(1)
