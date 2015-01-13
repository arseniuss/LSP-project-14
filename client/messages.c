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
	size_t ret = sprintf(msg, "r %s", username);

	correct_message(msg, 1, 1);

	return ret + 1;
}

size_t create_start_message(char *msg, unsigned char id)
{
	size_t ret = sprintf(msg, "s %c ", id);

	correct_message(msg, 2, 1, 3);

	return ret;
}

size_t create_move_message(char *msg, char move, unsigned char id)
{
	size_t ret = sprintf(msg, "m %c %c ", move, id);

	correct_message(msg, 3, 1, 3, 5);

	return ret;
}

size_t create_leave_message(char *msg, unsigned char id)
{
	size_t ret = sprintf(msg, "l %c ", id);

	correct_message(msg, 2, 1, 3);

	return ret;
}

int decode_accept_message(const char *msg, ssize_t len)
{
	debug_message(msg, len);

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
	int i, j;
	const char *ptr;

	snake_count = msg[1];

	if (snake_count > MAX_PLAYER_COUNT) {
		debugf("Too big snake count: %d", snake_count);
		return;
	}

	debugf("field %d %d\n", client_config.width, client_config.height);

	ptr = msg + 3;
	for (i = 0; i < snake_count; i++) {
		snake[i].id = *ptr++;
		snake[i].points[0].x = *ptr++;
		snake[i].points[0].y = *ptr++;

		debugf("snake 0 [%d, %d] %c\n", snake[i].points[0].x,
			snake[i].points[0].y, *ptr);
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

			debugf("snake %d [%d, %d] %c\n", j, snake[i].points[j].x,
				snake[i].points[j].y, *ptr);
		}
		snake[i].len = j - 1;
		ptr++;
	}

	food_count = *ptr++;
	ptr++;
	debugf("food count %d\n", food_count);

	for (i = 0; i < food_count; i++, ptr += 2) {
		food[i].x = *ptr;
		food[i].y = *(ptr + 1);
		debugf("food %d [%d, %d]\n", i, food[i].x, food[i].y);
	}

	debugf("Decoded STATE message\n");
}

void decode_message(const char *msg, ssize_t len)
{
	debug_message(msg, len);

	switch (msg[0]) {
	case STATE_MSG_CHAR:
		decode_state_message(msg, len);
		break;
	}
}
