TARGET_ARCHITECTURE ?= native
MARCH ?= -march=$(TARGET_ARCHITECTURE)
MTUNE ?= -mtune=$(TARGET_ARCHITECTURE)

CXX = g++
CC = gcc
CXXFLAGS = -O3 -Wall -Wextra -Wshadow -pedantic -std=c++14 -Iinclude -DX86_ASSEMBLY -DX64_ASSEMBLY -g2 -fomit-frame-pointer -fno-math-errno -flto $(MARCH) $(MTUNE)
CFLAGS = -O3 -Wall -Wextra -Wshadow -pedantic -std=c11 -Iinclude -DX86_ASSEMBLY -DX64_ASSEMBLY -g2 -fomit-frame-pointer -fno-math-errno -flto $(MARCH) $(MTUNE)
AR = gcc-ar
ARFLAGS = rcs

SOURCES = $(shell find -\( -name '*.cpp' -or -name '*.c' -\) -and -not -path './examples/*')
OBJECTS = $(patsubst %.cpp,%.o,$(patsubst %.c,%.o,$(SOURCES)))
EXAMPLES = $(patsubst %.cpp,%,$(wildcard examples/*.cpp))

LIB = libdake.a

.PHONY: all clean distclean


all: $(LIB) $(EXAMPLES)

$(LIB): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

%.o: %.cpp Makefile
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c Makefile
	$(CC) $(CFLAGS) -c $< -o $@

examples/%: examples/%.cpp $(LIB)
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIB) -lm

clean:
	$(RM) $(OBJECTS) $(EXAMPLES) .hdrdeps

distclean: clean
	$(RM) $(LIB)

.hdrdeps: $(SOURCES)
	$(CXX) $(CXXFLAGS) -MM $^ > $@

.hdrdeps: $(shell find -name '*.hpp' -and -not -path './include/dake/math/fmatrix/*')

-include .hdrdeps
