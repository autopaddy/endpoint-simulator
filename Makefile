CXXBIN=epsim
CXXTESTBIN=unittest

INC=include

# The C++ Compiler
CXX=g++

# Linker flags
LDFLAGS = -lpthread -lPocoFoundation -lPocoNet -lavahi-common -lavahi-client

# Used in compilation and linking for C++
CXXFLAGS = -Iinclude -std=gnu++17 -Wall -Wextra

# Used to automatically determine build dependencies
DEPFLAGS = -MT $@ -MMD -MP

SRC := $(filter-out %Test.cpp, $(wildcard ./src/*.cpp))
OBJ := $(SRC:%.cpp=%.o)
DEP := $(OBJ:%.o=%.d)

# Combine objects with test objects.
# Ensure main.cpp is removed to avoid mulitple mains
TEST := $(filter-out %main.o, $(OBJ)) $(filter %Test.cpp, $(wildcard ./src/*.cpp))

all: $(CXXBIN)

$(CXXBIN): $(OBJ)
	$(CXX) $(OBJ) $(CXXFLAGS) $(LDFLAGS) -o $@

$(CXXTESTBIN): $(TEST)
	$(CXX) $(TEST) $(CXXFLAGS) $(LDFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(DEPFLAGS) $(LDFLAGS) $< -o $@

.PHONY: test
test: $(CXXTESTBIN)
	./$(CXXTESTBIN)

.PHONY: clean
clean:
	rm -rf **/*.o
	rm -rf **/*.d
	rm -rf $(CXXBIN)
	rm -rf $(CXXTESTBIN)



