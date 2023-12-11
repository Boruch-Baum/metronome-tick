CC := clang
LDFLAGS := -lasound
CFLAGS := -Wall -Wextra -Werror -pedantic
DEBUG_CFLAGS := $(CFLAGS) -fsanitize=address,leak,undefined
RELEASE_CFLAGS := $(CFLAGS) -O3 -fomit-frame-pointer -flto

TARGET_NAME := tick
TARGET_DIR := target
DEBUG_DIR := $(TARGET_DIR)/debug
RELEASE_DIR := $(TARGET_DIR)/release

LIB := $(wildcard lib/*.c)
DEBUG_OBJS := $(LIB:%.c=$(DEBUG_DIR)/%.o)
RELEASE_OBJS := $(LIB:%.c=$(RELEASE_DIR)/%.o) $(RELEASE_DIR)/tick.o

.PHONY: build release clean

build: $(DEBUG_OBJS) $(DEBUG_DIR)/tick.o
	$(CC) $(DEBUG_CFLAGS) -o $(DEBUG_DIR)/$(TARGET_NAME) $^ $(LDFLAGS)

release: $(RELEASE_OBJS)
	$(CC) $(RELEASE_CFLAGS) -o $(RELEASE_DIR)/$(TARGET_NAME) $^ $(LDFLAGS)

$(DEBUG_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(DEBUG_CFLAGS) -MMD -MP -c $< -o $@

$(RELEASE_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(RELEASE_CFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -rf $(TARGET_DIR)
