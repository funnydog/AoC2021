CFLAGS := -O2 -Wall -Werror -std=c99
CXXFLAGS := -O2 -Wall -Werror -std=c++17
CPPFLAGS := -D_XOPEN_SOURCE=700
LDFLAGS :=
LDLIBS := -lfmt -lstdc++

.PHONY: all clean

all: day22 day22c day22-2 day22c-2

day22: day22.o

day22-2: day22-2.o

day22c: day22c.o

day22c-2: day22c-2.o

clean:
	rm -f *.o day22 day22c day22-2 day22c-2
