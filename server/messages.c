/*
 * File:   messages.c
 * Author: Ilze Dzene-Vanaga
 *	   Armands Skolmeisters
 *
 * Server message creation
 */

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "../common/Structures.h"
#include "../common/Messages.h"

#include "Server.h"

void parse_message(size_t len, char *msg)
{
	size_t i;
	for (i = 0; i < len; ++i)
	{
		if (msg[i] == '\r')
		{
			msg[i] = '\0';
		}
	}
}

size_t create_yes_message(char *msg, unsigned char id)
{
	size_t len;

	len = sprintf(msg, "a\ry\r%c\r%d\r%d", id, game_config.field_width,
		game_config.field_height);

	parse_message(len, msg);

	return len;
}

size_t create_no_message(char *msg)
{
	size_t len;

	len = sprintf(msg, "a\rn");

	return len;
}

size_t create_end_message(char *msg, unsigned char id)
{
	size_t len;

	len = sprintf(msg, "e\r%c", id);

	parse_message(len, msg);

	return len;
}

size_t create_dead_message(char *msg)
{
	size_t len;

	len = sprintf(msg, "d");

	return len;
}
