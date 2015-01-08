#!/usr/bin/make

#
# File:   Makefile
# Author: Armands Skolmeisters
#

# C Compiler
CC = gcc

# C compiler flags
CFLAGS = -Wall \
    $(shell pkg-config --cflags glib-2.0)

DEBUG ?= 0
ifeq ($(DEBUG), 1)
    CFLAGS += -DDEBUG
endif

# Linker flags
LDFLAGS = \
    $(shell pkg-config --libs glib-2.0)

COMMON_SRC = \
    common/errors.c \
    common/messages.c

# List of source files for client
CLIENT_SRC = \
    client/main.c

# List of source files for server
SERVER_SRC = \
    server/config.c \
    server/main.c   \
    server/messages.c \
    server/states.c

CLIENT_OBJS = $(CLIENT_SRC:.c=.o) $(COMMON_SRC:.c=.o)
SERVER_OBJS = $(SERVER_SRC:.c=.o) $(COMMON_SRC:.c=.o)
	
-include .depend

all: snake-client snake-server

setup:
	sudo apt-get install libglib2.0-dev
	
snake-client: $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o snake-client $(CLIENT_OBJS) $(LDFLAGS)
	
snake-server: $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o snake-server $(SERVER_OBJS) $(LDFLAGS)
		
%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
	$(CC) -MM -MT $@ $(CFLAGS) $< >> .depend
	
clean:
	rm -f snake-client snake-server client/*.o server/*.o .depend
