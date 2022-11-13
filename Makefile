TARGET_EXEC ?= isms
BUILD_DIR ?= ./build
SRC_DIRS ?= ./src

SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

#CPPFLAGS = $(INC_FLAGS) -MMD -MP -ggdb
CFLAGS=-I/usr/include -I/usr/local/include \
  -L/usr/local/lib -lSDL2 -llua -lm -ldl -llo -lmonome -lasound -ludev -levdev \
  -std=c11 -Wall -pthread -D_GNU_SOURCE

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

.PHONY: install
install:
	@echo ">> installing libs to /usr/local/share/isms"
	$(MKDIR_P) /usr/local/share/isms/system
	cp lua/* /usr/local/share/isms/system/
	@echo ">> installing binary to /usr/local/bin"
	cp build/isms /usr/local/bin/

.PHONY: run
run: $(BIN)
	./build/isms

