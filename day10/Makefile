CFLAGS := -O2 -Wall -Werror -std=c99
CXXFLAGS := -O2 -Wall -Werror -std=c++17
CPPFLAGS := -D_XOPEN_SOURCE=700
LDFLAGS :=
LDLIBS := -lfmt -lstdc++

.PHONY: all clean

all: day10 day10c

day10: day10.o

day10c: day10c.o

clean:
	rm -f *.o day10 day10c
