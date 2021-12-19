CFLAGS := -O2 -Wall -Werror -std=c99
CXXFLAGS := -O2 -Wall -Werror -std=c++17
CPPFLAGS := -D_XOPEN_SOURCE=700
LDFLAGS :=
LDLIBS := -lfmt -lstdc++ -lm

.PHONY: all clean

all: day19 day19c

day19: day19.o

day19c: day19c.o

clean:
	rm -f *.o day19 day19c
