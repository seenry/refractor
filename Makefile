CC = @echo "  CC :: "$(TARGET); clang++

EXEC = refractor

TARGET = 
DID_CLEAN = 0

BUILD = build
SRC = $(shell find ./src/ -name "*.cpp")
OBJS = $(patsubst %,$(BUILD)/%, $(SRC:.cpp=.o))
INC = -I./
LNK = 

FLAGS = -Wall -g -O2

all: $(EXEC)

$(BUILD)/%.o: %.cpp | libraries build_dir
	$(eval TARGET=$(patsubst build/./%,%,$(@)))
	$(CC) $(FLAGS) $(INC) $(CC_FLAGS) -c $< -o $@

$(EXEC): $(OBJS)
	$(eval TARGET=$(EXEC))
	$(CC) $(FLAGS) $(LNK) -o $@ $^

.PHONY: clean

clean:
	@echo "  cleaning...";  \
	rm -f $(EXEC); \
	rm -rf $(BUILD)

build_dir:
	@if [ ! -d "./$(BUILD)" ]; then \
		mkdir $(BUILD);             \
		echo "  mkdir $(BUILD)";    \
	fi
	@if [ ! -d "./$(BUILD)/src" ]; then \
		mkdir $(BUILD)/src;             \
		echo "  mkdir $(BUILD)/src";    \
	fi

ifeq ("$(wildcard $(PWD)/parlaylib)", "")
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
	@if [[ -nz "$(CLONE_REPO)" ]] ; then \
		git submodule update --init --recursive ; \
	fi
	@if [[ -nz "$(MK_BUILD_DIR)" ]] ; then \
		mkdir parlaylib/build ; \
	fi
	@if [[ -nz "$(INSTALL_LIB)" ]] ; then \
		cd parlaylib/build ; \
		cmake .. -DCMAKE_INSTALL_PREFIX:PATH=$(PWD)/parlaylib/build/install ; \
		cmake --build . --target install ; \
	fi

