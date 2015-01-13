/*
 * File:   Messages.h
 * Author: arseniuss
 *
 * Messages
 */

#ifndef MESSAGES_H
#define	MESSAGES_H

#define REGISTER_MESSAGE		1
#define START_MESSAGE			2

#define ACCEPT_MSG_CHAR			'a'
#define DEAD_MSG_CHAR			'd'
#define END_MSG_CHAR			'e'
#define STATE_MSG_CHAR			'o'

#define ACCEPT_MSG_UNACCEPTED		'n'
#define ACCEPT_MSG_ACCEPTED		'y'

#define STATE_MSG_UP_CHAR		'w'
#define STATE_MSG_RIGHT_CHAR		'd'
#define STATE_MSG_DOWN_CHAR		's'
#define STATE_MSG_LEFT_CHAR		'a'

void correct_message(char *msg, int count, ...);
void debug_message(const char *msg, int len);

#endif	/* MESSAGES_H */
