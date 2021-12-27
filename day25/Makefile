CFLAGS := -O2 -Wall -Werror -std=c99
CXXFLAGS := -O2 -Wall -Werror -std=c++17
CPPFLAGS := -D_XOPEN_SOURCE=700
LDFLAGS :=
LDLIBS := -lfmt -lstdc++

.PHONY: all clean

all: day25 day25c

day25: day25.o

day25c: day25c.o

clean:
	rm -f *.o day25 day25c
