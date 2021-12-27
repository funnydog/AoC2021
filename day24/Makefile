CFLAGS := -O2 -Wall -Werror -std=c99
CXXFLAGS := -O2 -Wall -Werror -std=c++17
CPPFLAGS := -D_XOPEN_SOURCE=700 -D_GNU_SOURCE
LDFLAGS :=
LDLIBS := -lfmt -lstdc++

.PHONY: all clean

all: day24 day24c

day24: day24.o

day24c: day24c.o

clean:
	rm -f *.o day24 day24c
