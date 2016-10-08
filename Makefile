TARGET_ARCHITECTURE ?= native
MARCH ?= -march=$(TARGET_ARCHITECTURE)
MTUNE ?= -mtune=$(TARGET_ARCHITECTURE)

CXX = g++
CC = gcc
CXXFLAGS = -O3 -Wall -Wextra -Wshadow -pedantic -std=c++14 -Iinclude -g2 -fomit-frame-pointer -fno-math-errno -flto $(MARCH) $(MTUNE) $(EXTRA_CXXFLAGS)
CFLAGS = -O3 -Wall -Wextra -Wshadow -pedantic -std=c11 -Iinclude -g2 -fomit-frame-pointer -fno-math-errno -flto $(MARCH) $(MTUNE) $(EXTRA_CFLAGS)
ifeq ($(CXX),g++)
	AR = gcc-ar
else
	AR = ar
endif
ARFLAGS = rcs

CXXSOURCES = $(shell find -name '*.cpp' -and -not -path './examples/*')
CSOURCES = $(shell find -name '*.c' -and -not -path './examples/*')
OBJECTS = $(patsubst %.cpp,%.o,$(CXXSOURCES)) $(patsubst %.c,%.o,$(CSOURCES))
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

.hdrdeps: $(CSOURCES) $(CXXSOURCES)
	$(CXX) $(CXXFLAGS) -MM $(CXXSOURCES) > $@
	$(CC) $(CFLAGS) -MM $(CSOURCES) > $@

.hdrdeps: $(shell find -name '*.hpp' -and -not -path './include/dake/math/fmatrix/*')

-include .hdrdeps
