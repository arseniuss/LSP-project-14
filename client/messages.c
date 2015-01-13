/*
 * File:   messages.c
 * Author: Armands Skolmeisters
 *
 * Client message creation
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "Client.h"
#include "../common/Defs.h"
#include "../common/Messages.h"

size_t create_register_message(char *msg, const char *username)
{
	size_t ret = sprintf(msg, "r\r%s\r", username);

	correct_message(msg);

	return ret;
}

size_t create_start_message(char *msg, unsigned char id)
{
	size_t ret = sprintf(msg, "s\r%c\r", id);

	correct_message(msg);

	return ret;
}

size_t create_move_message(char *msg, char move, unsigned char id)
{
	size_t ret = sprintf(msg, "m\r%c\r%c\r", move, id);

	correct_message(msg);

	return ret;
}

size_t create_leave_message(char *msg, unsigned char id)
{
	size_t ret = sprintf(msg, "l\r%c\t", id);

	correct_message(msg);

	return ret;
}

int decode_accept_message(const char *msg, ssize_t len)
{
	if (msg[0] != ACCEPT_MSG_CHAR && msg[1] != '\0' && strlen(msg) != 9) {
		debugf("Error in ACCEPT message format!");
		return -1;
	}

	if (msg[2] == ACCEPT_MSG_UNACCEPTED) {
		return 0;
	}

	if (client_config.id != 0) {
		debugf("User %s ID is already set to %c!\n",
			client_config.username, client_config.id);
		return -2;
	}

	client_config.id = msg[4];

	client_config.width = msg[6];

	client_config.height = msg[8];

	return 1;
}

void decode_state_message(const char *msg, ssize_t len)
{
	int i, j, snake_cnt = msg[1];
	const char *ptr;

	if (snake_count > MAX_PLAYER_COUNT) {
		debugf("Error in max player count");
		return;
	}

	ptr = msg + 3;
	for (i = 0; i <= snake_cnt; i++) {
		snake[i].id = *ptr++;
		snake[i].points[0].x = *ptr++;
		snake[i].points[0].y = *ptr++;

		debugf("Snake no. %d start at %d %d\n", i,
			snake[i].points[0].x, snake[i].points[0].y);

#ifdef DEBUG
		if (snake[i].points[0].x > client_config.width ||
			snake[i].points[0].x > client_config.height)
			debugf("Error in snake no. %d starting points!\n", i);
#endif

		for (j = 1; *ptr != '\0'; j++, ptr++) {
			switch (*ptr) {
			case STATE_MSG_UP_CHAR:
				snake[i].points[j].x = snake[i].points[j - 1].x;
				snake[i].points[j].y = snake[i].points[j - 1].y - 1;
				break;
			case STATE_MSG_RIGHT_CHAR:
				snake[i].points[j].x = snake[i].points[j - 1].x + 1;
				snake[i].points[j].y = snake[i].points[j - 1].y;
				break;
			case STATE_MSG_DOWN_CHAR:
				snake[i].points[j].x = snake[i].points[j - 1].x;
				snake[i].points[j].y = snake[i].points[j - 1].y + 1;
				break;
			case STATE_MSG_LEFT_CHAR:
				snake[i].points[j].x = snake[i].points[j - 1].x - 1;
				snake[i].points[j].y = snake[i].points[j - 1].y;
				break;
			default:
				debugf("Error in snake coordinates! Got char %c\n", *ptr);
				return;
			}

			debugf("snake [%d, %d] %c\n", snake[i].points[j].x,
				snake[i].points[j].y, *ptr);
		}
		snake[i].len = j - 1;
		ptr++;
	}

	for (i = 0; *ptr != '\0'; i++, ptr += 2) {
		food[i].x = *ptr;
		food[i].y = *(ptr + 1);
	}

	/* Save global snake_count */
	snake_count = snake_cnt;

	debugf("Decoded STATE message\n");
}

void decode_message(const char *msg, ssize_t len)
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

	switch (msg[0]) {
	case STATE_MSG_CHAR:
		decode_state_message(msg, len);
		break;
	}
}
