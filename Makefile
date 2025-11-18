CC = @echo " CC :: "$@ ; clang++

EXE := monty

COMMON_FLAGS := -Wall -Wextra -Wno-sign-compare -std=c++17
CFLAGS := $(COMMON_FLAGS) -O2 
DFLAGS := $(COMMON_FLAGS) -O0 -g -fsanitize=address -fsanitize=undefined
INC := -I$(PWD)/parlaylib/build/install/include -I$(PWD)/include
LNK := -lm

SRCS := $(shell find ./src/ -name '*.cpp')
OBJS := $(patsubst ./src/%.cpp,build/%.o,$(SRCS))

TEST_CPP := $(shell find ./test/ -name '*.cpp')
TEST_OS := $(filter-out ./test/build/s/main.o, $(patsubst ./src/%.cpp,./test/build/s/%.o,$(SRCS)))
TEST_OT := $(patsubst ./test/%.cpp,./test/build/t/%.o,$(TEST_CPP))

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) $(LNK) $(CFLAGS) $^ -o $@

build/%.o: src/%.cpp | libraries
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

debug: CFLAGS = $(DFLAGS)
debug: $(EXE)

test/build/s/%.o: src/%.cpp | libraries
	@mkdir -p $(dir $@)
	@$(CC) $(DFLAGS) $(INC) -c $< -o $@
test/build/t/%.o: test/%.cpp
	@mkdir -p $(dir $@)
	@$(CC) $(DFLAGS) $(INC) -I$(PWD)/test -c $< -o $@
run_tests: $(TEST_OS) $(TEST_OT)
	@$(CC) $(LNK) $(DFLAGS) $^ -o run_tests

.PHONY: all run_tests clean debug libraries

clean:
	@echo "  cleaning...";  \
	rm -f monty; \
	rm -rf build/ ; \
	rm -rf test/build ; \
	rm -f run_tests

libraries:
	@if [ ! -f parlaylib/CMakeLists.txt ]; then \
		git submodule update --init --recursive ; \
	fi
	@if [ ! -d parlaylib/build/install ]; then \
		mkdir -p parlaylib/build ; \
		cd parlaylib/build && \
		cmake .. -DCMAKE_INSTALL_PREFIX:PATH=$(PWD)/parlaylib/build/install && \
		cmake --build . --target install ; \
	fi
