CFLAGS := -O2 -Wall -Werror -std=c99
CXXFLAGS := -O2 -Wall -Werror -std=c++17
CPPFLAGS := -D_XOPEN_SOURCE=700
LDFLAGS :=
LDLIBS := -lfmt -lstdc++

.PHONY: all clean

all: day20 day20c

day20: day20.o

day20c: day20c.o

clean:
	rm -f *.o day20 day20c
