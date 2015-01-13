/*
 * File:   states.c
 * Author: Ilze Dzene-Vanaga
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
#include <time.h>
#include <stdlib.h>

#include "../common/Defs.h"
#include "../common/Defaults.h"
#include "Server.h"

struct Snake snakes[MAX_PLAYER_COUNT];
char moves[MAX_PLAYER_COUNT];
char got_food[MAX_PLAYER_COUNT];
struct Point new_head[MAX_PLAYER_COUNT];
struct Point food[MAX_FOOD_AMOUNT];
char field[MAX_GAME_HEIGHT + 2][MAX_GAME_WIDTH + 2];
int array_size;
char message[MAX_MESSAGE_SIZE];
char orig_message[MAX_MESSAGE_SIZE];

void start_game()
{
	array_size = MAX_SCORE_LIMIT + MAX_SNAKE_INITIAL_SIZE + 1;
	int i;	
	for (i = 0; i < server_config.max_players; i++)
	{
		got_food[i] = 'N';
	}
	
	//inicializē čūskas tiem spēlētājiem, kas aktīvi
	if (players[0].state == 1)
	{
		snakes[0].state = 1;
		snakes[0].size = game_config.initial_size;
		snakes[0].head_idx = 0;
		snakes[0].tail_idx = snakes[0].head_idx + snakes[0].size - 1;
		snakes[0].dir = 'r';
		moves[0] = 'r';
		for (i = 0; i < snakes[0].size; i++)
		{
			snakes[0].points[i].x = 1;
			snakes[0].points[i].y = snakes[0].size - i;
			snakes[0].string[i] = 'l';
		}
	}
	else
	{
		snakes[0].state = 0;
	}
	if (players[1].state == 1)
	{
		snakes[1].state = 1;
		snakes[1].size = game_config.initial_size;
		snakes[1].head_idx = 0;
		snakes[1].tail_idx = snakes[1].head_idx + snakes[1].size - 1;
		snakes[1].dir = 'd';
		moves[1] = 'd';
		for (i = 0; i < snakes[1].size; i++)
		{
			snakes[1].points[i].x = snakes[0].size - i;
			snakes[1].points[i].y = game_config.field_width;
			snakes[1].string[i] = 'u';
		}
	}
	else
	{
		snakes[1].state = 0;
	}
	if (players[2].state == 1)
	{
		snakes[2].state = 1;
		snakes[2].size = game_config.initial_size;
		snakes[2].head_idx = 0;
		snakes[2].tail_idx = snakes[2].head_idx + snakes[2].size - 1;
		snakes[2].dir = 'l';
		moves[2] = 'l';
		for (i = 0; i < snakes[2].size; i++)
		{
			snakes[2].points[i].x = game_config.field_height;
			snakes[2].points[i].y = game_config.field_width + snakes[0].size - i;
			snakes[2].string[i] = 'r';
		}
	}
	else
	{
		snakes[2].state = 0;
	}
	if (players[3].state == 1)
	{
		snakes[3].state = 1;
		snakes[3].size = game_config.initial_size;
		snakes[3].head_idx = 0;
		snakes[3].tail_idx = snakes[3].head_idx + snakes[3].size - 1;
		snakes[3].dir = 'u';
		moves[3] = 'u';
		for (i = 0; i < snakes[3].size; i++)
		{
			snakes[3].points[i].x = game_config.field_height + snakes[0].size - i;
			snakes[3].points[i].y = 1;
			snakes[3].string[i] = 'd';
		}
	}
	else
	{
		snakes[3].state = 0;
	}
	
	//iezīmē laukuma robežas, aizpilda laukumu ar space simboliem
	int x, y;	
	for (y = 0; y < game_config.field_width+2; y++)
	{
		field[0][y] = 'B';
		field[game_config.field_height+1][y] = 'B';
	} 	
	for (x = 0; x < game_config.field_height+2; x++)
	{
		field[x][0] = 'B';		
		for (y = 1; y < game_config.field_width+1; y++)
		{
			field[x][y] = ' ';	
		}
		field[x][game_config.field_width+1] = 'B';
	}
	
	//āboli
	srand(time(NULL));		
	for (i = 0; i < game_config.food_amount; i++)
	{
		get_random_food(i);
	}
}

void clear_game()
{
	int i;	
	for (i = 0; i < server_config.max_players; i++)
	{
		snakes[i].state = 0;
		players[i].points = 0;
	}
}

void end_game()
{
	game_state = STATE_INITIAL;	
	int i;
	int count = 0;
	for (i = 0; i < server_config.max_players; i++)
	{
		if (players[i].state == 1) count++;
	}
	if (count == 0)
	{
		printf("Send end message\n");		
		send_end_message(&players[0].addr, players[0].id);
	}
}

void get_random_food(int i)
{
	int rx, ry;
	rx = (rand() % game_config.field_height - 1) + 1;
	ry = (rand() % game_config.field_width - 1) + 1;	
	if (field[rx][ry] == ' ')
	{		
		food[i].x = (unsigned char)rx;
		printf("esma x: %d\n", rx); 
		printf("esma y: %d\n", ry);
		food[i].y = (unsigned char)ry;		
		field[food[i].x][food[i].y] = 'A';
	}
	else
	{
		get_random_food(i);
	}
}

void remove_player(int i)
{
	infof("User '%s' removed from game", players[i].username);	
	players[i].state = 0;
	players[i].id = '\0';
	players[i].username[0] = '\0';
	snakes[i].state = 2;		
}

void player_move(int i, char m)
{
	moves[i] = m;	
	infof("player move: %c\n", m);
}

void move_all()
{
	int i;
	int count = 0;
	//iezīmē kartē iespējamo galvu un pārbauda, vai nav tur, kur nevajag būt
	//vienīgā problēmsituācija - vienā laukā divas iespējamās galvas tiek risināta nākamajā daļā	
	for (i = 0; i < server_config.max_players; i++)
	{	
		if (players[i].state != 1) {continue;}
		count++;
		valid_move(i);			
		get_new_head(i);
		switch (field[new_head[i].x][new_head[i].y])
		{
			case 'A' : //Barība
				got_food[i] = 'Y';
				field[new_head[i].x][new_head[i].y] = 'P';
				break;
			case 'B' : //Laukuma mala
				snakes[i].state = 2;
				break;
			case 'H' : //Čūskas galva
				snakes[i].state = 2;
				break;
			case 'T' : //Čūskas ķermenis
				snakes[i].state = 2;
				break;
			case 'P' :
				snakes[i].state = 2;
				break;
			case 'D' :
				snakes[i].state = 2;
				field[new_head[i].x][new_head[i].y] = 'D'; //otra čūska izbeigsies nākamajā daļā
				break;
			case ' ' :
				field[new_head[i].x][new_head[i].y] = 'P';
				break;
		}			
	}

	//Ja vairs nav spēlētāju spēlē
	if (count == 0)
	{
		printf("beidzam!\n");		
		end_game();
		return;
	}
	
	//zīmē izmaiņas, izņem iepriekšējā daļā neizņemtās čūskas
	for (i = 0; i < server_config.max_players; i++)
	{	
		if (snakes[i].state != 1) {continue;}		
		if (field[new_head[i].x][new_head[i].y] == 'P')
		{
			field[new_head[i].x][new_head[i].y] = 'H';
			field[snakes[i].points[snakes[i].head_idx].x][snakes[i].points[snakes[i].head_idx].y] = 'T';
			snakes[i].head_idx = get_new_head_idx(i);
			snakes[i].points[snakes[i].head_idx].x = new_head[i].x;
			snakes[i].points[snakes[i].head_idx].y = new_head[i].y;
			snakes[i].string[snakes[i].head_idx] = get_opposite_dir(i);
			if (got_food[i] == 'Y')
			{
				got_food[i] = 'N';
				players[i].points++;
			}
			else
			{
				snakes[i].tail_idx = get_new_tail_idx(i);
				snakes[i].string[snakes[i].tail_idx] = '\0';
				field[snakes[i].points[snakes[i].tail_idx].x][snakes[i].points[snakes[i].tail_idx].y] = ' ';
			}
		}
		else
		{
			field[new_head[i].x][new_head[i].y] = ' ';			
			snakes[i].state = 2;
		}	
	}

	//nodzēš mirušās čūskas
	for (i = 0; i < server_config.max_players; i++)
	{	
		if (snakes[i].state == 2)
		{
			int d;			
			for (d = snakes[i].head_idx; d <= snakes[i].tail_idx; d++)
			{
				if (d > MAX_SCORE_LIMIT + MAX_SNAKE_INITIAL_SIZE)
				{
					d = 0;
				}
				field[snakes[i].points[d].x][snakes[i].points[d].y] = ' ';
			}
			snakes[i].state = 0;
			players[i].state = 2;
			send_dead_message(&players[i].addr);
		}
	}			
 	
	//atjauno ābolus
	for (i = 0; i < game_config.food_amount; i++)
	{
		if (field[food[i].x][food[i].y] != (unsigned char)'A')	
		{
			printf("Field is: %c\n", field[food[i].x][food[i].y]);			
			get_random_food(i);
		}
	}
	
	//izsūta ziņas
	send_messages();
}

void valid_move(int i)
{
	if ((snakes[i].dir == 'u' || snakes[i].dir == 'd') && (moves[i] == 'l' || moves[i] == 'r'))
	{
		snakes[i].dir = moves[i];
	}
	if ((snakes[i].dir == 'l' || snakes[i].dir == 'r') && (moves[i] == 'u' || moves[i] == 'd'))
	{
		snakes[i].dir = moves[i];
	}
}

char get_opposite_dir(int i)
{
	switch (snakes[i].dir)
	{
		case 'u' :
			return 'd';
			break;
		case 'd' :
			return 'u';
			break;
		case 'l' :
			return 'r';
			break;
		case 'r' :
			return 'l';
			break;
		default :
			return ' ';
	}
}

void get_new_head(int i)
{	
	if (snakes[i].dir == 'u')
	{
		new_head[i].x = snakes[i].points[snakes[i].head_idx].x - 1;
		new_head[i].y = snakes[i].points[snakes[i].head_idx].y;
	}
	if (snakes[i].dir == 'd')
	{
		new_head[i].x = snakes[i].points[snakes[i].head_idx].x + 1;
		new_head[i].y = snakes[i].points[snakes[i].head_idx].y;
	}
	if (snakes[i].dir == 'l')
	{
		new_head[i].x = snakes[i].points[snakes[i].head_idx].x;
		new_head[i].y = snakes[i].points[snakes[i].head_idx].y - 1;
	}
	if (snakes[i].dir == 'r')
	{
		new_head[i].x = snakes[i].points[snakes[i].head_idx].x;
		new_head[i].y = snakes[i].points[snakes[i].head_idx].y + 1;
	}
}

int get_new_head_idx(int i)
{
	if (snakes[i].head_idx == 0)
	{
		return MAX_SCORE_LIMIT + MAX_SNAKE_INITIAL_SIZE;
	}
	else
	{
		return snakes[i].head_idx - 1;
	}
}

int get_new_tail_idx(int i)
{
	if (snakes[i].tail_idx == 0)
	{
		return MAX_SCORE_LIMIT + MAX_SNAKE_INITIAL_SIZE;
	}
	else
	{
		return snakes[i].tail_idx - 1;
	}
}

void send_messages()
{
	int len;
	int player_count = count_active_players();
	int i;
	message[0] = 'o';
	message[1] = (unsigned char)player_count;
	message[2] = '\0';
	len = 3;
	int player_string_len;
	for (i = 0; i < server_config.max_players; i++)
	{
		if (players[i].state == 1)
		{
			player_string_len = get_player_string(i, &message[len]);
			len += player_string_len;
		}
	}
	message[len] = (unsigned char)game_config.food_amount;
	len++;
	message[len] = '\0';
	len++;
	for (i = 0; i < game_config.food_amount; i++)
	{
		message[len] = food[i].x - 1;
		len++;		
		message[len] = food[i].y - 1;
		len++;
	}
	message[len] = '\0';
	len++;
	for (i = 0; i < server_config.max_players; i++)
	{
		if (players[i].state == 1 || players[i].state == 2)
		{
			sendto(listener, message, len, 0, (struct sockaddr *) &players[i].addr, sizeof(players[i].addr));
		}
	}
}

int count_active_players()
{
	int count = 0;
	int i;
	for (i = 0; i < server_config.max_players; i++)
	{
		if (players[i].state == 1)
		{
			count++;
		}
	}
	return count;
}

int get_player_string(int i, char * buffer)
{
	int code_len;
	buffer[0] = players[i].id;
	buffer[1] = (unsigned char)(snakes[i].points[snakes[i].head_idx].x - 1);
	buffer[2] = (unsigned char)(snakes[i].points[snakes[i].head_idx].y - 1);
	buffer[3] = (unsigned char)(snakes[i].size);
	code_len = get_snake_coded(i, &buffer[4]);
	buffer[4+code_len] = '\0';
	return code_len + 5;
}

int get_snake_coded(int i, char * buffer)
{
	int len_orig = get_snake_original(i, orig_message);
	int buffer_idx = 0;	
	int k = 0;
	char c;
	int count = 0;
	while (k < len_orig)
	{
		c = orig_message[k];
		k++;
		count = 1;
		while (orig_message[k] == c)
		{
			count++;
			k++;
		}
		buffer[buffer_idx] = (unsigned char) count;
		buffer[buffer_idx+1] = c;
		buffer_idx += 2;
	}
	return buffer_idx;
}

int get_snake_original(int i, char * buffer)
{
	int k = 0;
	int c;	
	for (c = snakes[i].head_idx; c < snakes[i].tail_idx; c++)
	{
		if (c > MAX_SCORE_LIMIT + MAX_SNAKE_INITIAL_SIZE)
		{
			c = 0;
		}
		buffer[k] = snakes[i].string[c];
		k++;
	}
	buffer[k] = '\0';
	return k;
}

void send_dead_message(struct sockaddr_in *addr)
{
	size_t len = create_dead_message(message);
	send_message(addr, len, message);
}

void send_end_message(struct sockaddr_in *addr, unsigned char id)
{
	size_t len = create_end_message(message, id);
	send_message(addr, len, message);
}






