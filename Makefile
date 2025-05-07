CC=gcc
CFLAGS=-W -Wall -Wextra -O3 -DNDEBUG -march=native
LDFLAGS=

EXE=main
SRC=main.c sha256.c

$(EXE): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

.PHONY: run clean

build: $(EXE)

test: build file.txt
	./$(EXE) file.txt

file.txt:
	echo -ne "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq" > file.txt

clean:
	rm -rf $(EXE) file.txt
