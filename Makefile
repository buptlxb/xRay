SHELL := /bin/bash
BIN := xRay

DISASM := disasm
BUILD_DIR := build
SRC_DIR := src

INCLUDE := -I./include -I./include/beaengine
CFLAGS := -g -O0 #-O3 
CXX := g++ 
CC := gcc

##############################################

DISASM_CFLAGS := -pedantic -ansi -pipe -fno-common -fshort-enums -g -Wall -W -Wextra -Wconversion -Wno-long-long \
	-Wshadow -Wpointer-arith -Wcast-qual -Wcast-align -Wwrite-strings 
DISASM_OBJ := $(BUILD_DIR)/BeaEngine.o
DISASM_OBJ_S := BeaEngine.o
DISASM_DEP := $(BUILD_DIR)/BeaEngine.d
DISASM_SOURCE := $(DISASM)/BeaEngine.c

SRC1 := $(wildcard $(SRC_DIR)/*.cpp)
SRC1 += $(wildcard $(SRC_DIR)/*.c)
SRC := $(subst $(SRC_DIR)/,,$(SRC1))
OBJ1 := $(patsubst %.cpp,%.o,${SRC1})
OBJ2 := $(patsubst %.c,%.o,${OBJ1})
OBJ := $(subst $(SRC_DIR)/,$(BUILD_DIR)/,$(OBJ2))
OBJ_S := $(subst $(SRC_DIR)/,,$(OBJ2))
DEP1 := $(patsubst %.cpp,%.d,${SRC1})
DEP2 := $(patsubst %.c,%.d,${DEP1})
DEP := $(subst $(SRC_DIR)/,$(BUILD_DIR)/,$(DEP2))

.PHONY: all clean disasm 

vpath %.cpp $(SRC_DIR)
vpath %.c $(SRC_DIR)
vpath %.d $(BUILD_DIR)
vpath %.o $(BUILD_DIR)

all: $(DISASM_OBJ_S) $(OBJ_S)
	@if \
	$(CXX) $(OBJ) $(DISASM_OBJ) $(LIBS) $(CFLAGS) -o $(BIN);\
	then echo -e "[\e[32;1mLINK\e[m] \e[33m$(OBJ) $(DISASM_OBJ)\e[m \e[36m->\e[m \e[32;1m$(BIN)\e[m"; \
	else echo -e "[\e[31mFAIL\e[m] \e[33m$(OBJ) $(DISASM_OBJ)\e[m \e[36m->\e[m \e[32;1m$(BIN)\e[m"; exit -1; fi;

-include $(DEP) 
-include $(DISASM_DEP)

$(BUILD_DIR)/%.d: %.cpp
	@if [ ! -d $(BUILD_DIR) ]; then \
	mkdir $(BUILD_DIR); fi;
	@if \
	$(CXX) ${INCLUDE} -MM $< > $@;\
	then echo -e "[\e[32mCXX \e[m] \e[33m$<\e[m \e[36m->\e[m \e[33;1m$@\e[m"; \
	else echo -e "[\e[31mFAIL\e[m] \e[33m$<\e[m \e[36m->\e[m \e[33;1m$@\e[m"; exit -1; fi;

$(BUILD_DIR)/%.d: %.c
	@if [ ! -d $(BUILD_DIR) ]; then \
	mkdir $(BUILD_DIR); fi;
	@if \
	$(CC) ${INCLUDE} -MM $< > $@;\
	then echo -e "[\e[32mCC  \e[m] \e[33m$<\e[m \e[36m->\e[m \e[33;1m$@\e[m"; \
	else echo -e "[\e[31mFAIL\e[m] \e[33m$<\e[m \e[36m->\e[m \e[33;1m$@\e[m"; exit -1; fi;

$(DISASM_DEP): $(DISASM_SOURCE)
	@if [ ! -d $(BUILD_DIR) ]; then \
	mkdir $(BUILD_DIR); fi;
	@if \
	$(CC) ${INCLUDE} -MM $< > $@;\
	then echo -e "[\e[32mCC  \e[m] \e[33m$<\e[m \e[36m->\e[m \e[33;1m$@\e[m"; \
	else echo -e "[\e[31mFAIL\e[m] \e[33m$<\e[m \e[36m->\e[m \e[33;1m$@\e[m"; exit -1; fi;


%.o: %.cpp
	@if \
	$(CXX) ${CFLAGS} ${INCLUDE} -c $< -o $(BUILD_DIR)/$@; \
	then echo -e "[\e[32mCXX \e[m] \e[33m$<\e[m \e[36m->\e[m \e[33;1m$(BUILD_DIR)/$@\e[m"; \
	else echo -e "[\e[31mFAIL\e[m] \e[33m$<\e[m \e[36m->\e[m \e[33;1m$(BUILD_DIR)/$@\e[m"; exit -1; fi;

%.o: %.c
	@if \
	$(CC) ${CFLAGS} ${INCLUDE} -c $< -o $(BUILD_DIR)/$@; \
	then echo -e "[\e[32mCC  \e[m] \e[33m$<\e[m \e[36m->\e[m \e[33;1m$(BUILD_DIR)/$@\e[m"; \
	else echo -e "[\e[31mFAIL\e[m] \e[33m$<\e[m \e[36m->\e[m \e[33;1m$(BUILD_DIR)/$@\e[m"; exit -1; fi;

$(DISASM_OBJ_S): 
	@if \
	$(CC) $(CFLAGS) ${DISASM_CFLAGS} ${INCLUDE} -c $(DISASM_SOURCE) -o $(DISASM_OBJ); \
	then echo -e "[\e[32mCC  \e[m] \e[33m$(DISASM_SOURCE)/BeaEngine.c\e[m \e[36m->\e[m \e[33;1m$(DISASM_OBJ)\e[m"; \
	else echo -e "[\e[31mFAIL\e[m] \e[33m$(DISASM_SOURCE)/BeaEngine.c\e[m \e[36m->\e[m \e[33;1m$(DISASM_OBJ)\e[m"; exit -1; fi;


clean:
	@echo -e "[\e[32mCLEAN\e[m] \e[33m$(BIN) $(BUILD_DIR)\e[m"
	@rm -rf $(BIN) build $(DISASM_OBJ) 

