CC = @echo " CC :: "$@ ; clang++

EXE := refractor

COMMON_FLAGS := -Wall -Wextra
CFLAGS := $(COMMON_FLAGS) -O2 
DFLAGS := $(COMMON_FLAGS) -O0 -g -fsanitize=address -fsanitize=undefined
INC := -I$(PWD)/parlaylib/build/install/include -I$(PWD)/include
LNK := 

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
	rm -f $(EXEC); \
	rm -rf build/ ; \
	rm -rf test/build

ifeq ("$(wildcard $(PWD)/parlaylib/CMakeLists.txt)", "")
CLONE_REPO = 1
endif
ifeq ("$(wildcard $(PWD)/parlaylib/build)", "")
MK_BUILD_DIR = 1
endif
ifeq ("$(wildcard $(PWD)/parlaylib/build/install)", "")
INSTALL_LIB = 1
endif
libraries:
	@cd $(PWD)
	@if [ -n "$(CLONE_REPO)" ] ; then \
		git submodule update --init --recursive ; \
	fi
	@if [ -n "$(MK_BUILD_DIR)" ] ; then \
		mkdir parlaylib/build ; \
	fi
	@if [ -n "$(INSTALL_LIB)" ] ; then \
		cd parlaylib/build ; \
		cmake .. -DCMAKE_INSTALL_PREFIX:PATH=$(PWD)/parlaylib/build/install ; \
		cmake --build . --target install ; \
	fi
