CXX = clang++
IDIRENGINE = $(shell find -type d | grep 'src\|include' | awk '{print"-I"$$0}' )
override CXXFLAGS += -g -Wno-everything -std=c++17 $(IDIRENGINE)

SRCS = $(shell find . -name '.ccls-cache' -type d -prune -o -type f -name '*.cpp' -print)
OBJS = $(patsubst %.cpp, %.o, $(SRCS))

main: 
	$(CXX) $(CXXFLAGS) $(SRCS) -o main

clean:
	rm -f $(OBJS) main

