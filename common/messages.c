/*
 * File:   common/errors.c
 * Author: Armands Skolmeisters
 *
 * Messages
 */

#include <string.h>

#include "Defs.h"

void message_send(int fd, const char *msg)
{
	int n;

	n = write(fd, msg, strlen(msg));
	if (n < 0)
		error("ERROR in message sending");
}
