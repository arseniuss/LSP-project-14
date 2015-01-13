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

size_t create_yes_message(char *msg, unsigned char id)
{
	size_t len;

	len = sprintf(msg, "a\ry\r%c\r%d\r%d\r", id, game_config.field_width,
		game_config.field_height);
	correct_message(msg);

	return len;
}

size_t create_no_message(char *msg)
{
	size_t len;

	len = sprintf(msg, "a\rn\r");
	correct_message(msg);

	return len;
}

size_t create_end_message(char *msg, unsigned char id)
{
	size_t len;

	len = sprintf(msg, "e\r%c\r", id);
	correct_message(msg);

	return len;
}

size_t create_dead_message(char *msg)
{
	size_t len;

	len = sprintf(msg, "d\r");
	correct_message(msg);

	return len;
}
