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
SRC_DIR := ../src
INC_DIR := ../inc
BLD_DIR := .
LIB_DIR := $(if $(wildcard ../../../lib),../../../lib,../lib)

### Source Files #######################
SRCS := $(wildcard $(SRC_DIR)/*.c)

### Compiler Flags #####################
CFLAGS := -compiler=sdcc --vc -clib=sdcc_iy -SO3 --opt-code-size -pragma-include:$(INC_DIR)/zpragma.inc
CFLAGS += -I$(INC_DIR)
CFLAGS += -I$(LIB_DIR)/libdrv/inc
CFLAGS += -I$(LIB_DIR)/libzxn/inc

# select CRT
CFLAGS += -startup=30

# verbose output
# CFLAGS += -v

ifeq ($(BUILD), debug)
# create list files
CFLAGS += --list

# add C code to list files
CFLAGS += --c-code-in-asm

# create debug code
CFLAGS += -D__DEBUG__
else
# tradeoff speed vs. code quality
CFLAGS += --max-allocs-per-node200000
endif

### Linker Flags #######################
LDFLAGS := -subtype=$(APPTYPE) -Cz"--clean" -create-app -o $(BLD_DIR)/$(APPNAME)
LDFLAGS += -L$(LIB_DIR)/libdrv/build -llibdrv
LDFLAGS += -L$(LIB_DIR)/libzxn/build -llibzxn

ifeq ($(BUILD), debug)
# create map file
LDFLAGS += -m

# create symbol files
LDFLAGS += -s
endif

### Compiler Command ###################
CC = zcc

### Build Target #######################
all: libdrv libzxn
	$(CC) +$(TARGET) $(CFLAGS) $(SRCS) $(LDFLAGS)
ifeq ($(APPTYPE), dotn)
ifeq ($(OS),Windows_NT)
#	@$(RM) $(BLD_DIR)/$(APPNAME)
#	@$(MV) $(BLD_DIR)/ping $(BLD_DIR)/$(APPNAME)
endif
endif

libdrv:
	$(MAKE) -C $(LIB_DIR)/libdrv/build BUILD=$(BUILD)

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
	@$(MAKE) -C $(LIB_DIR)/libdrv/build clean
	@$(MAKE) -C $(LIB_DIR)/libzxn/build clean
