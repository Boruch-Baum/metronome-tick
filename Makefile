CC := clang
LDFLAGS := -lasound
CFLAGS := -Wall -Wextra -Werror -pedantic
DEBUG_CFLAGS := $(CFLAGS) -fsanitize=address,leak,undefined
RELEASE_CFLAGS := $(CFLAGS) -O3 -fomit-frame-pointer -flto

TARGET_NAME := tick
TARGET_DIR := target
DEBUG_DIR := $(TARGET_DIR)/debug
RELEASE_DIR := $(TARGET_DIR)/release
DOC_DIR := $(TARGET_DIR)/doc

LIB_SRCS := $(wildcard lib/*.c)
TEST_SRCS := $(wildcard test/*.c)
DOC_SRCS := $(wildcard doc/*.scd)
BUILD_OBJS := $(LIB_SRCS:%.c=$(DEBUG_DIR)/%.o) $(DEBUG_DIR)/main.o
TEST_OBJS := $(LIB_SRCS:%.c=$(DEBUG_DIR)/%.o) $(TEST_SRCS:%.c=$(DEBUG_DIR)/%.o)
RELEASE_OBJS := $(LIB_SRCS:%.c=$(RELEASE_DIR)/%.o) $(RELEASE_DIR)/main.o
DOC_OUTPUTS := $(DOC_SRCS:%.scd=$(TARGET_DIR)/%)

PREFIX?=/usr/local
BINDIR?=$(PREFIX)/bin
MANDIR?=$(PREFIX)/share/man

.PHONY: build test release doc run clean

build: $(BUILD_OBJS)
	$(CC) $(DEBUG_CFLAGS) -o $(DEBUG_DIR)/$(TARGET_NAME) $^ $(LDFLAGS)

test: $(TEST_OBJS)
	$(CC) $(DEBUG_CFLAGS) -o $(DEBUG_DIR)/$(TARGET_NAME)_test $^ $(LDFLAGS)
	@./$(DEBUG_DIR)/$(TARGET_NAME)_test

release: $(RELEASE_OBJS)
	$(CC) $(RELEASE_CFLAGS) -o $(RELEASE_DIR)/$(TARGET_NAME) $^ $(LDFLAGS)

doc: $(DOC_OUTPUTS)

$(DEBUG_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(DEBUG_CFLAGS) -MMD -MP -c $< -o $@

$(RELEASE_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(RELEASE_CFLAGS) -MMD -MP -c $< -o $@

$(TARGET_DIR)/%: %.scd
	mkdir -p $(dir $@)
	scdoc < $< > $@

run: build
	@./$(DEBUG_DIR)/$(TARGET_NAME)

install: release doc
	mkdir -p $(DESTDIR)$(BINDIR) $(DESTDIR)$(MANDIR)/man1 $(DESTDIR)$(MANDIR)/man5
	install -m755 $(RELEASE_DIR)/$(TARGET_NAME) $(DESTDIR)$(BINDIR)/$(TARGET_NAME)
	install -m644 $(DOC_DIR)/tick.1 $(DESTDIR)$(MANDIR)/man1/tick.1
	install -m644 $(DOC_DIR)/tick-config.5 $(DESTDIR)$(MANDIR)/man5/tick-config.5
	install -m644 $(DOC_DIR)/tick-presets.5 $(DESTDIR)$(MANDIR)/man5/tick-presets.5

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/$(TARGET_NAME) \
		$(DESTDIR)$(MANDIR)/man1/tick.1 \
		$(DESTDIR)$(MANDIR)/man5/tick-config.5 \
		$(DESTDIR)$(MANDIR)/man5/tick-presets.5

clean:
	rm -rf $(TARGET_DIR)
