CXX = g++
IDIRENGINE = $(shell find -type d | grep 'src\|include' | awk '{print"-I"$$0}' )
override CXXFLAGS += -Wall -Wextra -std=c++17 $(IDIRENGINE) -pthread

SRCS = $(shell find . -name '.ccls-cache' -type d -prune -o -type f -name '*.cpp' -print)
OBJS = $(patsubst %.cpp, %.o, $(SRCS))




main: debug64
	

debug32:
	$(CXX) $(CXXFLAGS) $(SRCS) -g  -m32 -o ./Debug/ACEngine
	
release32:
	$(CXX) $(CXXFLAGS) $(SRCS) -m32 -O3 -o ./Release/ACEngine

debug64:
	$(CXX) $(CXXFLAGS) $(SRCS) -g  -m64 -o ./x64/Debug/ACEngine

release64:
	$(CXX) $(CXXFLAGS) $(SRCS) -m64 -O3 -o ./x64/Release/ACEngine



clean:
	rm -f $(OBJS) main
	rm -f ./Debug/ACEngine
	rm -f ./Release/ACEngine
	rm -f ./x64/Debug/ACEngine
	rm -f ./x64/Release/ACEngine
