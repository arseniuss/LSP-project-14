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

	pthread_create (&thread_timer, NULL, (void *) &timer_thread_function, NULL);
	pthread_create (&thread_listener, NULL, (void *) &listener_thread_function, NULL);
	
	game_state = STATE_INITIAL;
	
	do_move = 0;
	while(1)
	{
		if (do_move)
		{
			do_move = 0;
			move_all();
			send_messages();
		}
	}

	return(EXIT_SUCCESS);
}

void timer_thread_function()
{
	int i = 0;	
	for (i = 0; i < 30; i++)	
	{
		sleep(3);
		printf("pagāja 3 sekundes\n");
	}
}

