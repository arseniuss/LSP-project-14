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
	int i, j, snake_count, food_count;
	const char *ptr;
	unsigned char id, x, y;

	snake_count = msg[1];

	if (snake_count > MAX_PLAYER_COUNT) {
		debugf("Too big snake count: %d", snake_count);
		return;
	}

	memset(field, DEFAULT_BLANK_CHAR, sizeof(field));
	debugf("field %d %d\n", client_config.width, client_config.height);

	ptr = msg + 3;
	for (i = 0; i < snake_count; i++) {
		id = *ptr++;
		x = *ptr++;
		y = *ptr++;

		field[x + y * client_config.width] = i + 1;

		for (j = 1; *ptr != '\0'; j++, ptr++) {
			switch (*ptr) {
			case STATE_MSG_UP_CHAR:
				y--;
				break;
			case STATE_MSG_RIGHT_CHAR:
				x++;
				break;
			case STATE_MSG_DOWN_CHAR:
				y++;
				break;
			case STATE_MSG_LEFT_CHAR:
				x--;
				break;
			default:
				debugf("Error in snake coordinates! Got char %c\n", *ptr);
				return;
			}
			field[x + y * client_config.width] = i + 1;
		}
		ptr++;
	}

	food_count = *ptr++;
	ptr++;
	debugf("food count %d\n", food_count);

	for (i = 0; i < food_count; i++, ptr += 2) {
		x = *ptr;
		y = *(ptr + 1);
		field[x + y * client_config.width] = 'f' + i;
	}

	debugf("Decoded STATE message\n");

	client_config.state = PLAYER_STATE_ACTIVE;
}

void decode_dead_message(const char *msg, ssize_t len)
{
	if (msg[0] == DEAD_MSG_CHAR && msg[1] == '\0')
		client_config.state = PLAYER_STATE_DEAD;
}

void decode_end_message(const char *msg, ssize_t len)
{
	if (msg[0] == END_MSG_CHAR && msg[1] == '\0') {
		client_config.winner = msg[2];
		client_config.state = PLAYER_STATE_END;
	}
}

void decode_message(const char *msg, ssize_t len)
{
	debug_message(msg, len);

	switch (msg[0]) {
	case STATE_MSG_CHAR:
		decode_state_message(msg, len);
		break;
	case DEAD_MSG_CHAR:
		decode_dead_message(msg, len);
		break;
	case END_MSG_CHAR:
		decode_end_message(msg, len);
		break;
	}
}
