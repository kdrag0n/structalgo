CC = clang
CFLAGS = -Wall -Wpedantic -O2 -std=gnu99 -g -lm

SRC_DIR = src
OUT_DIR = out

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
