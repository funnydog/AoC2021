CFLAGS := -O2 -Wall -Werror -std=c99
CXXFLAGS := -O2 -Wall -Werror -std=c++17
LDFLAGS :=
LDLIBS := -lfmt -lstdc++

.PHONY: clean all

all: day6 day6c

day6: day6.o

day6c: day6c.o

clean:
	rm -f *.o day6 day6c
