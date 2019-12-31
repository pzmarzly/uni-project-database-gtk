CC ?= gcc
CFLAGS = -ggdb -std=c11 -xc -Wall -Wextra `pkg-config --cflags gtk+-3.0`
LDFLAGS = -lm `pkg-config --libs gtk+-3.0` \
	-fstack-protector-all -fsanitize=undefined \
	-fsanitize=address -fno-omit-frame-pointer
OBJ_DIR ?= obj
BIN_DIR ?= bin

default: $(BIN_DIR)/wez-mnie-gtk
all: $(BIN_DIR)/wez-mnie-gtk $(BIN_DIR)/test

WEZ_MNIE_GTK_SRC = main.c Repo.c Editor.c Recent.c Welcome.c Utils.c
WEZ_MNIE_GTK = ${WEZ_MNIE_GTK_SRC:%.c=$(OBJ_DIR)/%.o}
$(BIN_DIR)/wez-mnie-gtk: ${WEZ_MNIE_GTK} | $(BIN_DIR)
	$(CC) ${WEZ_MNIE_GTK} ${LDFLAGS} -o $@

TEST_SRC = test.c Repo.c RepoString.c Utils.c
TEST = ${TEST_SRC:%.c=$(OBJ_DIR)/%.o}
$(BIN_DIR)/test: ${TEST} | $(BIN_DIR)
	$(CC) ${TEST} ${LDFLAGS} -o $@

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	$(CC) ${CFLAGS} -c -MMD $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

.PHONY: clean
clean:
	-rm -f wez-mnie-gtk test
	-rm -rf $(OBJ_DIR)

-include *.d
