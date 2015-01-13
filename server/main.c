/*
 * File:   main.c
 * Author: Armands Skolmeisters
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <string.h>

#include "../common/Defs.h"
#include "Server.h"

int game_state;
int listener;
struct Player players[MAX_PLAYER_COUNT];
char send_buffer[MAX_MESSAGE_SIZE];
int do_move;

int main(int argc, char** argv)
{
	pthread_t thread_timer;
	pthread_t thread_listener;	
	
	parse_config();
	
	shuffle_ids();
	
	clear_players();
	
	
	pthread_create (&thread_timer, NULL, (void *) &timer_thread_function, NULL);
	pthread_create (&thread_listener, NULL, (void *) &listener_thread_function, NULL);
	
	game_state = STATE_INITIAL;
	
	do_move = 0;
	while(1)
	{
		if (do_move == 1 && game_state == STATE_GAME)
		{
			do_move = 0;
			move_all();
			//send_messages();
		}
	}
	

	return(EXIT_SUCCESS);
}

void timer_thread_function()
{	
	struct timespec ts;
	int milisec = 1000 / game_config.snake_speed;
	ts.tv_sec = 0;
	ts.tv_nsec = milisec * 1000000L;	
	infof("Timer thread working");	
	while (1)
	{
		while (game_state == STATE_GAME)	
		{
			infof("do move");
			do_move = 1;
			nanosleep(&ts, (struct timespec *)NULL);
		}
		printf("Iztīra spēli");		
		clear_game();
		while (game_state == STATE_INITIAL)
		{
		}		
		start_game();
		send_messages();
	}
}

void clear_players()
{
	int i;	
	for (i = 0; i < MAX_PLAYER_COUNT; i++)
	{
		players[i].id = '\0';
		players[i].username[0] = '\0';
		players[i].state = 0;
		players[i].points = 0;
	}
}

