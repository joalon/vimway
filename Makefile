# Mostly a formality for building libnvim. TODO: Remove or make it useful
CC = gcc

.PHONY: libnvim

clean:
	rm include/libnvim.a

libnvim:
	cd third-party/neovim; make libnvim; cp build/lib/libnvim.a ../../include/

