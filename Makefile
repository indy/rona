# make          defaults to make guest
# make guest    compile the shared library containing game code
# make host     compile the host that hot-reloads the guest
# make all      compile guest + host
# make run      run the game
# make test     compile and run the unit tests
# make clean    clean target directory
# make tags     create TAGS file for emacs
# make fmt      format c code

CC=gcc
INCLUDE_FLAGS=-Iext

EXT_SRC=ext/*.h

GAME_SRC=src/*.c
GAME_HEADERS=src/*.h

SHADERS_OUT=./target/tile.vert.c ./target/tile.frag.c ./target/screen.vert.c ./target/screen.frag.c ./target/editor.vert.c ./target/editor.frag.c

# the shared library with the game code
#
GUEST_NAME=guest
GUEST_MAIN=src/guest/guest.c
GUEST_CFLAGS=-std=c99 -fPIC -g -O0 -D _DEBUG -D FPL_DEBUG
# GUEST_CFLAGS=-std=c99 -fPIC -g0 -O3 -D FPL_RELEASE
GUEST_LDFLAGS=-shared
GUEST_OUT=./target/lib$(GUEST_NAME).so

# the host that hot-reloads the shared library
#
HOST_CFLAGS=-Wno-pointer-arith -Wno-narrowing -g -O0 -D _DEBUG -D FPL_DEBUG
# HOST_CFLAGS=-Wno-narrowing -g0 -O3 -D FPL_RELEASE
HOST_MAIN=src/host/host.cpp
HOST_HEADERS=src/rona.h src/rona_gl.h
HOST_LIBS=-lGL -lm -lpthread -ldl -lrt -lX11 -lstdc++
HOST_OUT=./target/host

# misc. unit tests
#
TEST_OUT=./target/test
TEST_SRC=src/tests/test.c
TEST_CFLAGS= -g -O0
TEST_LIBS=-lm

TAGS_OUT=./TAGS

guest: $(GUEST_OUT)
host: $(HOST_OUT)
test: $(TEST_OUT)
tags: $(TAGS_OUT)

./target/tile.vert.c: src/shaders/tile.vert
	xxd -i $< > $@
./target/tile.frag.c: src/shaders/tile.frag
	xxd -i $< > $@
./target/screen.vert.c: src/shaders/screen.vert
	xxd -i $< > $@
./target/screen.frag.c: src/shaders/screen.frag
	xxd -i $< > $@
./target/editor.vert.c: src/shaders/editor.vert
	xxd -i $< > $@
./target/editor.frag.c: src/shaders/editor.frag
	xxd -i $< > $@

$(GUEST_OUT): $(GUEST_MAIN) $(GAME_SRC) $(EXT_SRC) $(GAME_HEADERS) Makefile $(SHADERS_OUT)
	$(CC) $(GUEST_CFLAGS) $(INCLUDE_FLAGS) $(GUEST_MAIN) -o $(GUEST_OUT) -Wall $(GUEST_LDFLAGS)

$(HOST_OUT): $(HOST_MAIN) $(HOST_HEADERS) Makefile
	$(CC) $(HOST_CFLAGS) -g -c $(INCLUDE_FLAGS) -DCR_DEPLOY_PATH=\"target\" -DCR_NAME=\"$(GUEST_NAME)\" $(HOST_MAIN) -o ./target/host.o
	$(CC) -o $(HOST_OUT) ./target/host.o -Wall $(INCLUDE_FLAGS) $(HOST_LIBS)

$(TEST_OUT): $(TEST_SRC) $(GAME_SRC) $(GAME_HEADERS) Makefile
	$(CC) $(TEST_CFLAGS) $(INCLUDE_FLAGS) -o $@ ext/munit/munit.c $(TEST_SRC) $(TEST_LIBS)
	$(TEST_OUT)

$(TAGS_OUT): $(GAME_SRC) $(GAME_HEADERS)
	find src -type f -print | xargs etags

fmt: $(GAME_SRC) $(GAME_HEADERS)
	find src -name '*.[ch]' | xargs clang-format -i -style=file

.PHONY: clean watch

clean:
	rm target/*

all: guest host

run: $(GUEST_OUT) $(HOST_OUT)
	$(HOST_OUT)

watch:
	while true; do inotifywait -e close_write --quiet --recursive src assets; make; done
