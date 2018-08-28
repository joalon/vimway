CC=gcc
CFLAGS=-DWLR_USE_UNSTABLE -lpixman-1 -lwlroots -lwayland-server -I include

SRC=src/
BUILD=build/
OBJ=$(SRC)vimway.o
BIN=$(BUILD)vimway

$(BIN): $(OBJ)
	$(CC) -o $(BIN) $(OBJ) $(CFLAGS)


clean:
	rm -f $(BIN) $(OBJ)
