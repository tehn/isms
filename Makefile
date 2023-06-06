TARGET_EXEC ?= isms
BUILD_DIR ?= ./build
SRC_DIRS ?= ./src

SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CFLAGS=-I/usr/include -I/usr/local/include -I/usr/include/lua5.4 \
  -std=c11 -O2 -Wall -pthread -D_GNU_SOURCE
LDFLAGS=-L/usr/local/lib -lSDL2 -llua5.4 -lm -ldl -llo

# main target (C)
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS) $(CFLAGS)

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(INC_FLAGS) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p

.PHONY: install-lib
install-lib:
	@echo ">> installing libs to ~/.local/share/isms"
	$(MKDIR_P) ~/.local/share/isms/system
	cp lua/* ~/.local/share/isms/system/

.PHONY: install
install:
	@echo ">> installing binary to /usr/local/bin"
	cp build/isms /usr/local/bin/
