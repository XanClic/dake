CXX ?= g++
CXXFLAGS = -O3 -Wall -Wextra -std=c++11 -Iinclude -DX86_ASSEMBLY -DX64_ASSEMBLY
AR = ar rcs

OBJECTS = $(patsubst %.cpp,%.o,$(shell find -name '*.cpp' -print0))

.PHONY: all clean


all: dake.a

dake.a: $(OBJECTS)
	$(AR) $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJECTS) dake.a
