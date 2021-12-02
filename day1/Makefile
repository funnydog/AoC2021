CFLAGS := -O2 -Wall -Werror -std=c11
CXXFLAGS := -O2 -Wall -Werror -std=c++17
CPPFLAGS :=
LDFLAGS :=
LDLIBS := -lfmt -lstdc++

.PHONY:	clean all

all: day1 day1c

day1: day1.o

day1c: day1c.o

clean:
	rm -f *.o day1 day1c
