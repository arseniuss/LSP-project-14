/*
 * File:   common/errors.c
 * Author: Armands Skolmeisters
 *
 * Messages
 */

#include <string.h>

#include "Defs.h"

void correct_message(char *msg)
{
	int i;

	for (i = 0; i < strlen(msg); ++i)
		if (msg[i] == '\r')
			msg[i] = '\0';
}
