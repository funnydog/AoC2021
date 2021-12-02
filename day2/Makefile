CFLAGS := -O2 -Wall -Werror -std=c99
CXXFLAGS := -O2 -Wall -Werror -std=c++17
LDFLAGS :=
LDLIBS := -lstdc++ -lfmt

.PHONY: clean all

all: day2 day2c

day2: day2.o

day2c: day2c.o

clean:
	rm -f *.o day2 day2c
