CFLAGS := -O2 -Wall -Werror -std=c99
CXXFLAGS := -O2 -Wall -Werror -std=c++17
CPPFLAGS := -D_XOPEN_SOURCE=700
LDFLAGS :=
LDLIBS := -lfmt -lstdc++ -lm

.PHONY: all clean

all: day17 day17c

day17: day17.o

day17c: day17c.o

clean:
	rm -f *.o day17 day17c
