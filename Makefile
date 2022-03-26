CXX = g++
IDIRENGINE = $(shell find -type d | grep 'src\|include' | awk '{print"-I"$$0}' )
override CXXFLAGS += -Wall -Wextra -std=c++17 $(IDIRENGINE) -pthread

SRCS = $(shell find . -name '.ccls-cache' -type d -prune -o -type f -name '*.cpp' -print)
OBJS = $(patsubst %.cpp, %.o, $(SRCS))




main: debug64
	

debug32:
	rm -f ./build/32Debug/ACEngine
	$(CXX) $(CXXFLAGS) $(SRCS) -g  -m32 -o ./build/32Debug/ACEngine
	
release32:
	rm -f ./build/32Release/ACEngine
	$(CXX) $(CXXFLAGS) $(SRCS) -m32 -O3 -o ./build/32Release/ACEngine

debug64:
	rm -f ./build/64Debug/ACEngine
	$(CXX) $(CXXFLAGS) $(SRCS) -g  -m64 -o ./build/64Debug/ACEngine

release64:
	rm -f ./build/64Release/ACEngine
	$(CXX) $(CXXFLAGS) $(SRCS) -m64 -O3 -o ./build/64Release/ACEngine



clean:
	rm -f $(OBJS) main
	rm -f ./build/32Debug/ACEngine
	rm -f ./build/32Release/ACEngine
	rm -f ./build/64Debug/ACEngine
	rm -f ./build/64Release/ACEngine
