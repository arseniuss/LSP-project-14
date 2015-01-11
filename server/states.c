/*
 * File:   states.c
 * Author: Armands Skolmeisters
 *
 * Server states
 */

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <string.h>

#include "../common/Defs.h"
#include "../common/Defaults.h"
#include "Server.h"

struct Point snakes[MAX_PLAYER_COUNT][DEFAULT_SCORE_LIMIT + 3];
char field[MAX_GAME_HEIGHT][MAX_GAME_WIDTH];

void start_game()
{
	if (players[0].state == 1)
	{
		snakes[0][0].x = 0;
		snakes[0][0].y = 2;
		snakes[0][1].x = 0;
		snakes[0][1].y = 1;
		snakes[0][2].x = 0;
		snakes[0][2].y = 0; 
	}
	if (players[1].state == 1)
	{
		snakes[1][0].x = 2;
		snakes[1][0].y = game_config.field_width - 1;
		snakes[1][1].x = 1;
		snakes[1][1].y = game_config.field_width - 1;
		snakes[1][2].x = 0;
		snakes[1][2].y = game_config.field_width - 1; 
	}
	if (players[2].state == 1)
	{
		snakes[1][0].x = game_config.field_height - 1;
		snakes[1][0].y = game_config.field_width - 3;
		snakes[1][1].x = game_config.field_height - 1;
		snakes[1][1].y = game_config.field_width - 2;
		snakes[1][2].x = game_config.field_height - 1;
		snakes[1][2].y = game_config.field_width - 1; 
	}
	if (players[3].state == 1)
	{
		snakes[1][0].x = game_config.field_height - 3;
		snakes[1][0].y = 0;
		snakes[1][1].x = game_config.field_height - 2;
		snakes[1][1].y = 0;
		snakes[1][2].x = game_config.field_height - 1;
		snakes[1][2].y = 0; 
	}
}

void remove_player(int i)
{
	players[i].state = 0;	
	infof("remove player\n");
}

void player_move(int i, unsigned char m)
{
	infof("player move: %c\n", m);
}

void move_all()
{
	
}

void send_messages()
{
	switch (game_state) {
	case STATE_GAME:
		break;
	}
}

size_t create_state_message(char *msg)
{
		


	size_t len;

	len = sprintf(msg, "d");

	return len;
}

int get_snake_as_string(int i)
{
	
}





