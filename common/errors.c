/*
 * File:   common/errors.c
 * Author: Armands Skolmeisters
 *
 * Error handling
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "../common/Defs.h"

void error(const char *msg, ...)
{
	char buf[BUFSIZ];
	va_list args;

	va_start(args, msg);

	vsnprintf(buf, BUFSIZ, msg, args);

	va_end(args);

	perror(buf);

	exit(1);
}
