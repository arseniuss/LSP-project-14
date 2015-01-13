/*
 * File:   common/errors.c
 * Author: Armands Skolmeisters
 *
 * Messages
 */

#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#include "../common/Messages.h"
#include "../common/Defs.h"

void correct_message(char *msg, int count, ...)
{
	int i;
	va_list args;

	va_start(args, count);

	for (i = 0; i < count; ++i)
		msg[va_arg(args, int)] = '\0';
	va_end(args);
}

void debug_message(const char *msg, int len)
{
#ifdef DEBUG
	int d;

	debugf("Got message: ");
	for (d = 0; d < len; ++d)
		if (msg[d] == 0)
			printf("\\0|");
		else if (!isgraph(msg[d]))
			printf("%d|", msg[d]);
		else
			printf("%c|", msg[d]);
	printf("\n");
#endif
}
