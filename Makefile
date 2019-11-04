CC=gcc
CFLAGS=-g -march=native -pipe -Wall -Werror=array-bounds -Werror=format-overflow=0 -Werror=format -Werror=implicit-function-declaration -Werror=implicit-int -Werror=incompatible-pointer-types -Wno-comment -Wno-switch -Wno-unused-variable -Werror=address -fsanitize=undefined -lm

all: packrat-analyzer

packrat-analyzer: packrat-analyzer.c
	$(CC) $(CFLAGS) -o packrat-analyzer packrat-analyzer.c

.PHONY: all
