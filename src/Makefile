CC=gcc
CFLAGS=-DWLR_USE_UNSTABLE -lpixman-1 -lwlroots -lwayland-server

OBJ=test-comp.o
BIN=test-comp

$(BIN): $(OBJ)
	$(CC) -o $(BIN) $(OBJ) $(CFLAGS)


clean:
	rm -f $(BIN) $(OBJ)
