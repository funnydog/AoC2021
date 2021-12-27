CFLAGS := -O2 -Wall -Werror -std=c99
CXXFLAGS := -O2 -Wall -Werror -std=c++17
CPPFLAGS := -D_XOPEN_SOURCE=700
LDFLAGS :=
LDLIBS := -lfmt -lstdc++

.PHONY: all clean

all: day23 day23c

day23: day23.o

day23c: day23c.o

clean:
	rm -f *.o day23 day23c
