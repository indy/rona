CC=gcc

INCLUDE_FLAGS=-Ithird_party

GUEST_NAME=guest
GUEST_CFLAGS=-std=c99 -fPIC -g -O0 -D _DEBUG
# GUEST_CFLAGS=-std=c99 -fPIC -g0 -O3
GUEST_LDFLAGS=-shared
GUEST_SRC=src/guest.c src/game.c
GUEST_HEADERS=src/game.h
GUEST_OUT=./target/lib$(GUEST_NAME).so

HOST_CFLAGS=-Wno-narrowing
HOST_LIBS=-lGL -lm -lpthread -ldl -lrt -lX11 -lstdc++
HOST_OUT=./target/host

guest: $(GUEST_OUT)

$(GUEST_OUT): $(GUEST_SRC) $(GUEST_HEADERS) Makefile
	$(CC) $(GUEST_CFLAGS) $(INCLUDE_FLAGS) $(GUEST_SRC) -o $(GUEST_OUT) $(GUEST_LDFLAGS)

host: $(HOST_OUT)

$(HOST_OUT): src/host.cpp Makefile
	$(CC) $(HOST_CFLAGS) -g -c $(INCLUDE_FLAGS) -DCR_DEPLOY_PATH=\"target\" -DCR_NAME=\"$(GUEST_NAME)\" src/host.cpp -o ./target/host.o
	$(CC) -o $(HOST_OUT) ./target/host.o -s -Wall $(INCLUDE_FLAGS) $(HOST_LIBS)

.PHONY: clean
clean:
	rm target/*
all: guest host

run:
	$(HOST_OUT)
