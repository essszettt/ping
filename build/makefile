.PHONY: all clean

### Target Platform ####################
TARGET := zxn

### Project Name #######################
APPNAME := ping

### Binary type ########################
APPTYPE := dotn

### OS specific settings ###############
ifeq ($(OS),Windows_NT)
RM := rm -f
MV := mv -f
else
RM := rm -f
MV := mv -f
endif

### Build Type #########################
BUILD ?= release

### Source Directories #################
LIB_DIR = ../lib
SRC_DIR = ../src
INC_DIR = ../inc
BLD_DIR = .

### Source Files #######################
SRCS := $(wildcard $(SRC_DIR)/*.c)

### Compiler Flags #####################
CFLAGS = -compiler=sdcc --vc -clib=sdcc_iy -SO3 --opt-code-size -I$(INC_DIR) -I$(LIB_DIR)/libzxn/inc -pragma-include:$(INC_DIR)/zpragma.inc

# select CRT
CFLAGS += -startup=30

# tradeoff speed vs. code quality
CFLAGS += --max-allocs-per-node200000

# verbose output
# CFLAGS += -v

ifeq ($(BUILD), debug)
# create list files
CFLAGS += --list

# add C code to list files
CFLAGS += --c-code-in-asm

# create debug code
CFLAGS += -D__DEBUG__
endif

### Linker Flags #######################
LDFLAGS = -L$(LIB_DIR)/libzxn/build -llibzxn -subtype=$(APPTYPE) -Cz"--clean" -create-app -o $(BLD_DIR)/$(APPNAME)

ifeq ($(BUILD), debug)
# create map file
LDFLAGS += -m

# create symbol files
LDFLAGS += -s
endif

### Compiler Command ###################
CC = zcc

### Build Target #######################
all: libzxn
	$(CC) +$(TARGET) $(CFLAGS) $(SRCS) $(LDFLAGS)
ifeq ($(APPTYPE), dotn)
ifeq ($(OS),Windows_NT)
#	@$(RM) $(BLD_DIR)/$(APPNAME)
#	@$(MV) $(BLD_DIR)/ping $(BLD_DIR)/$(APPNAME)
endif
endif

libzxn:
	$(MAKE) -C $(LIB_DIR)/libzxn/build BUILD=$(BUILD)

### Cleanup Build Files ################
clean:
	@$(RM) $(BLD_DIR)/$(APPNAME)
	@$(RM) $(wildcard $(BLD_DIR)/*.lis)
	@$(RM) $(wildcard $(BLD_DIR)/*.map)
	@$(RM) $(wildcard $(BLD_DIR)/*.sym)
	@$(RM) $(wildcard $(SRC_DIR)/*.lis)
	@$(RM) $(wildcard $(SRC_DIR)/*.sym)
	@$(MAKE) -C $(LIB_DIR)/libzxn/build clean
