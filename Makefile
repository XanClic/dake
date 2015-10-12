CXX ?= g++
CXXFLAGS = -O3 -Wall -Wextra -Wshadow -std=c++11 -Iinclude -DX86_ASSEMBLY -DX64_ASSEMBLY -g2
AR ?= ar
ARFLAGS = rcs

OBJECTS = $(patsubst %.cpp,%.o,$(shell find -name '*.cpp' -and -not -path './examples/*'))
EXAMPLES = $(patsubst %.cpp,%,$(wildcard examples/*.cpp))

LIB = libdake.a

.PHONY: all clean


all: $(LIB) $(EXAMPLES)

$(LIB): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

examples/%: examples/%.cpp $(LIB)
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIB) -lm

clean:
	$(RM) $(OBJECTS) $(LIB) $(EXAMPLES)
