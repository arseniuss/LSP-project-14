/*
 * File:   main.c
 * Author: Armands Skolmeisters
 *
 * Server message parsing
 */

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "../common/Structures.h"
#include "../common/Messages.h"

#include "Server.h"

const char *no_message = "a\0n\0";

size_t create_yes_message(char *buffer, unsigned char id)
{
	size_t i, len;

	len = sprintf(buffer, "a\ry\r%c\r%d\r%d", id, game_config.field_width,
		game_config.field_height);

	for (i = 0; i < len; ++i) {
		if (buffer[i] == '\r')
			buffer[i] = '\0';
	}

	return len;
}
