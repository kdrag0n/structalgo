CC = clang
CFLAGS = -Wall -Wpedantic -O2 -std=gnu99 -g $(LIBS)
LIBS = -lm

SRC_DIR = src
OUT_DIR = out

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	LIBS += -lbsd
endif

sources = $(wildcard $(SRC_DIR)/*.c)
targets = $(sources:$(SRC_DIR)/%.c=$(OUT_DIR)/%)

all: $(targets)

$(OUT_DIR)/%: $(SRC_DIR)/%.c
	@echo "  CC      $@"
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -o $@ $<

clean:
	@echo "  CLEAN   "
	@rm -f $(targets)

.PHONY: all clean
