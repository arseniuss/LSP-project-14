/*
 * File:   common/errors.c
 * Author: Armands Skolmeisters
 *
 * Error handling
 */

#include <stdio.h>
#include <stdlib.h>

void error(const char *msg)
{
	perror(msg);
	exit(1);
}
