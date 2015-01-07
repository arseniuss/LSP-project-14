#!/usr/bin/make

#
# File:   Makefile
# Author: Armands Skolmeisters
#

# C Compiler
CC = gcc

# C compiler flags
CFLAGS += -Wall

# List of source files for client
CLIENT_SRC = \
    client/main.c

# List of source files for server
SERVER_SRC = \
    server/main.c

CLIENT_OBJS = $(CLIENT_SRC:.c=.o)
SERVER_OBJS = $(SERVER_SRC:.c=.o)
	
-include .depend

all: snake-client snake-server
	
snake-client: $(CLIENT_OBJS)
	$(CC) $(CLIENT_OBJS) -o snake-client
	
snake-server: $(SERVER_OBJS)
	$(CC) $(SERVER_OBJS) -o snake-server
		
%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
	$(CC) -MM -MT $@ $(CFLAGS) $< >> .depend
	
clean:
	rm -f snake-client snake-server client/*.o server/*.o .depend
