PROGRAM = mx++
CXX = g++
CXXFLAGS = -g -std=c++17
CPP_FILES = $(wildcard src/*.cpp)
H_FILES = $(wildcard src/*.h)

LDOPTIONS = -L.

$(PROGRAM): $(CPP_FILES) $(H_FILES) 
	$(CXX) -o2 -o $(PROGRAM) $(CXXFLAGS) $(LDOPTIONS) $(CPP_FILES)
