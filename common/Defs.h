/*
 * File:   Defs.h
 * Author: arseniuss
 *
 * Definitions used in source
 */

#ifndef DEFS_H
#define	DEFS_H

#include <stdio.h>

#ifdef DEBUG
#define debugf(fmt, ...)		do {\
						fprintf(stdout, "[DEBUG] ");\
						fprintf(stdout, fmt, ##__VA_ARGS__);\
					} while(0)
#else
#define debugf(fmt, ...)		/* Nothing */
#endif

#define infof(fmt, ...)			do {\
						fprintf(stdout, "[INFO] ");\
						fprintf(stdout, fmt, ##__VA_ARGS__);\
						fprintf(stdout, "\n");\
					} while (0)

void error(const char *msg);

/* Messages */
void message_send(int fd, const char *msg);

#endif	/* DEFS_H */

