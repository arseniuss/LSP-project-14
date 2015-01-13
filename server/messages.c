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

	len = sprintf(msg, "a y %c %c %c ", id, game_config.field_width,
		game_config.field_height);
	correct_message(msg, 5, 1, 3, 5, 7, 9);

	return len;
}

size_t create_no_message(char *msg)
{
	size_t len;

	len = sprintf(msg, "a n ");
	correct_message(msg, 2, 1, 3);

	return len;
}

size_t create_end_message(char *msg, unsigned char id)
{
	size_t len;

	len = sprintf(msg, "e %c ", id);
	correct_message(msg, 2, 1, 3);

	return len;
}

size_t create_dead_message(char *msg)
{
	size_t len;

	len = sprintf(msg, "d ");
	correct_message(msg, 1, 1);

	return len;
}
