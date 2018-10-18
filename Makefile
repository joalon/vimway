CC = gcc
CFLAGS = -DWLR_USE_UNSTABLE -g
INC = -lpixman-1 -lwlroots -lwayland-server -lm -I include

OUT = build/vimway
ODIR = obj
SDIR = src

_OBJS = main.o vimway.o
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) -c $(INC) -o $@ $< $(CFLAGS)

$(OUT): $(OBJS)
	$(CC) -o $(OUT) $(CFLAGS) $(INC) $(OBJS)


.PHONY: clean

clean:
	rm $(ODIR)/*.o $(OUT)

libnvim:
	cd third-party/neovim; make libnvim; cp build/lib/libnvim.a ../../include/

