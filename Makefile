CC=gcc
CFLAGS=-g -march=native -pipe -Wall -Werror=array-bounds -Werror=format-overflow=0 -Werror=format -Werror=implicit-function-declaration -Werror=implicit-int -Werror=incompatible-pointer-types -Wno-comment -Wno-switch -Wno-unused-variable -Werror=address -fsanitize=undefined -lm

all: packrat-analyzer packrat-reader packrat-writer

packrat-analyzer: packrat-analyzer.c
	$(CC) $(CFLAGS) -o packrat-analyzer packrat-analyzer.c

packrat-reader: packrat-reader.c
	$(CC) $(CFLAGS) -o packrat-reader packrat-reader.c

packrat-writer: packrat-writer.c
	$(CC) $(CFLAGS) -o packrat-writer packrat-writer.c

.PHONY: all
