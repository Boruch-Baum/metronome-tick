CC := clang
LDFLAGS := -lasound
CFLAGS := -Wall -Wextra -Werror -pedantic
DEBUG_CFLAGS := $(CFLAGS) -fsanitize=address,leak,undefined
RELEASE_CFLAGS := $(CFLAGS) -O3 -fomit-frame-pointer -flto

TARGET_NAME := tick
TARGET_DIR := target
DEBUG_DIR := $(TARGET_DIR)/debug
RELEASE_DIR := $(TARGET_DIR)/release

LIB_SRCS := $(wildcard lib/*.c)
TEST_SRCS := $(wildcard test/*.c)
BUILD_OBJS := $(LIB_SRCS:%.c=$(DEBUG_DIR)/%.o) $(DEBUG_DIR)/main.o
TEST_OBJS := $(LIB_SRCS:%.c=$(DEBUG_DIR)/%.o) $(TEST_SRCS:%.c=$(DEBUG_DIR)/%.o)
RELEASE_OBJS := $(LIB_SRCS:%.c=$(RELEASE_DIR)/%.o) $(RELEASE_DIR)/main.o

.PHONY: build test release run clean

build: $(BUILD_OBJS)
	$(CC) $(DEBUG_CFLAGS) -o $(DEBUG_DIR)/$(TARGET_NAME) $^ $(LDFLAGS)

test: $(TEST_OBJS)
	$(CC) $(DEBUG_CFLAGS) -o $(DEBUG_DIR)/$(TARGET_NAME)_test $^ $(LDFLAGS)
	@./$(DEBUG_DIR)/$(TARGET_NAME)_test

release: $(RELEASE_OBJS)
	$(CC) $(RELEASE_CFLAGS) -o $(RELEASE_DIR)/$(TARGET_NAME) $^ $(LDFLAGS)

$(DEBUG_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(DEBUG_CFLAGS) -MMD -MP -c $< -o $@

$(RELEASE_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(RELEASE_CFLAGS) -MMD -MP -c $< -o $@

run: build
	@./$(DEBUG_DIR)/$(TARGET_NAME)

clean:
	rm -rf $(TARGET_DIR)
