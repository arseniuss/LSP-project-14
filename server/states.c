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
#include "../common/Messages.h"
#include "Server.h"

struct Snake snakes[MAX_PLAYER_COUNT];
char moves[MAX_PLAYER_COUNT];
char got_food[MAX_PLAYER_COUNT];
struct Point new_head[MAX_PLAYER_COUNT];
struct Point food[MAX_FOOD_AMOUNT];
char field[MAX_GAME_WIDTH + 2][MAX_GAME_HEIGHT + 2];
int array_size;
char message[MAX_MESSAGE_SIZE];
char orig_message[MAX_MESSAGE_SIZE];
unsigned char multiplayer_game;

void start_game()
{
	//taimeris
	alarm(game_config.time_limit * 60);

	array_size = MAX_SCORE_LIMIT + MAX_SNAKE_INITIAL_SIZE + 1;
	int i;
	int count = 0;
	for (i = 0; i < server_config.max_players; i++) {
		got_food[i] = 'N';
	}

	//inicializē čūskas tiem spēlētājiem, kas aktīvi
	if (players[0].state == 1) {
		count++;
		snakes[0].state = 1;
		snakes[0].size = game_config.initial_size;
		snakes[0].head_idx = 0;
		snakes[0].tail_idx = snakes[0].head_idx + snakes[0].size - 1;
		snakes[0].dir = STATE_MSG_RIGHT_CHAR;
		moves[0] = STATE_MSG_RIGHT_CHAR;
		for (i = 0; i < snakes[0].size; i++) {
			snakes[0].points[i].x = 1;
			snakes[0].points[i].y = snakes[0].size - i;
			snakes[0].string[i] = STATE_MSG_LEFT_CHAR;
		}
	} else {
		snakes[0].state = 0;
	}
	if (players[1].state == 1) {
		count++;
		snakes[1].state = 1;
		snakes[1].size = game_config.initial_size;
		snakes[1].head_idx = 0;
		snakes[1].tail_idx = snakes[1].head_idx + snakes[1].size - 1;
		snakes[1].dir = STATE_MSG_DOWN_CHAR;
		moves[1] = STATE_MSG_DOWN_CHAR;
		for (i = 0; i < snakes[1].size; i++) {
			snakes[1].points[i].y = snakes[0].size - i;
			snakes[1].points[i].x = game_config.field_width;
			snakes[1].string[i] = STATE_MSG_UP_CHAR;
		}
	} else {
		snakes[1].state = 0;
	}
	if (players[2].state == 1) {
		count++;
		snakes[2].state = 1;
		snakes[2].size = game_config.initial_size;
		snakes[2].head_idx = 0;
		snakes[2].tail_idx = snakes[2].head_idx + snakes[2].size - 1;
		snakes[2].dir = STATE_MSG_LEFT_CHAR;
		moves[2] = STATE_MSG_LEFT_CHAR;
		for (i = 0; i < snakes[2].size; i++) {
			snakes[2].points[i].y = game_config.field_height;
			snakes[2].points[i].x = game_config.field_width + snakes[0].size - i;
			snakes[2].string[i] = STATE_MSG_RIGHT_CHAR;
		}
	} else {
		snakes[2].state = 0;
	}
	if (players[3].state == 1) {
		count++;
		snakes[3].state = 1;
		snakes[3].size = game_config.initial_size;
		snakes[3].head_idx = 0;
		snakes[3].tail_idx = snakes[3].head_idx + snakes[3].size - 1;
		snakes[3].dir = STATE_MSG_UP_CHAR;
		moves[3] = STATE_MSG_UP_CHAR;
		for (i = 0; i < snakes[3].size; i++) {
			snakes[3].points[i].y = game_config.field_height + snakes[0].size - i;
			snakes[3].points[i].x = 1;
			snakes[3].string[i] = STATE_MSG_DOWN_CHAR;
		}
	} else {
		snakes[3].state = 0;
	}

	//pazīme, vai ir vairāk kā viens spēlētājs
	if (count > 1) {
		multiplayer_game = 1;
	} else {
		multiplayer_game = 0;
	}

	//iezīmē laukuma robežas, aizpilda laukumu ar space simboliem
	int x, y;
	for (x = 0; x < game_config.field_width + 2; x++) {
		field[x][0] = 'B';
		field[x][game_config.field_height + 1] = 'B';
	}
	for (y = 1; y < game_config.field_height + 2; y++) {
		field[0][y] = 'B';
		for (x = 1; x < game_config.field_width + 1; x++) {
			field[x][y] = ' ';
		}
		field[game_config.field_width + 1][y] = 'B';
	}

	//āboli
	srand(time(NULL));
	for (i = 0; i < game_config.food_amount; i++) {
		get_random_food(i);
	}
}

void clear_game()
{
	int i;
	for (i = 0; i < server_config.max_players; i++) {
		snakes[i].state = 0;
		players[i].points = 0;
		if (players[i].state == 2) {
			players[i].state = 1;
		}
	}
}

void end_game(int i)
{
	game_state = STATE_INITIAL;
	int c;
	if (i == -1) {
		infof("Game has ended with no winners");
		for (c = 0; c < server_config.max_players; c++) {
			if (players[c].state != 0) {
				send_end_message(&players[c].addr, ' ');
			}
		}
	} else {
		infof("Game has ended. Player %c has won!", players[i].id);
		for (c = 0; c < server_config.max_players; c++) {
			if (players[c].state != 0) {
				send_end_message(&players[c].addr, players[i].id);
			}
		}
	}
}

void end_game_time_limit()
{
	infof("Time limit reached");
	unsigned char max = 0;
	int max_idx = -1;
	int i;
	for (i = 0; i < server_config.max_players; i++) {
		if (players[i].state == 1) {
			if (players[i].points > max) {
				max = players[i].points;
				max_idx = i;
			}
		}
	}
	end_game(max_idx);
}

void get_random_food(int i)
{
	int rx, ry;
	rx = (rand() % game_config.field_width - 1) + 1;
	ry = (rand() % game_config.field_height - 1) + 1;
	if (field[rx][ry] == ' ') {
		food[i].x = (unsigned char) rx;
		food[i].y = (unsigned char) ry;
		field[food[i].x][food[i].y] = 'A';
	} else {
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
}

void move_all()
{
	int i;
	int count = 0;
	//iezīmē kartē iespējamo galvu un pārbauda, vai nav tur, kur nevajag būt
	//vienīgā problēmsituācija - vienā laukā divas iespējamās galvas tiek risināta nākamajā daļā
	for (i = 0; i < server_config.max_players; i++) {
		if (players[i].state != 1) {
			continue;
		}
		count++;
		valid_move(i);
		get_new_head(i);
		switch (field[new_head[i].x][new_head[i].y]) {
		case 'A': //Barība
			got_food[i] = 'Y';
			field[new_head[i].x][new_head[i].y] = 'P';
			break;
		case 'B': //Laukuma mala
			snakes[i].state = 2;
			break;
		case 'H': //Čūskas galva
			snakes[i].state = 2;
			break;
		case 'T': //Čūskas ķermenis
			snakes[i].state = 2;
			break;
		case 'P':
			snakes[i].state = 2;
			field[new_head[i].x][new_head[i].y] = 'D'; //otra čūska izbeigsies nākamajā daļā
			break;
		case 'D':
			snakes[i].state = 2;
			break;
		case ' ':
			field[new_head[i].x][new_head[i].y] = 'P';
			break;
		}
	}

	//Ja palicis viens (multiplayer) vai vairs nav spēlētāju (singleplayer)
	if (multiplayer_game == 1 && count < 2) {
		for (i = 0; i < server_config.max_players; i++) {
			if (snakes[i].state != 1) break;
		}
		end_game(i);
		return;
	}
	if (multiplayer_game == 0 && count < 1) {
		end_game(-1);
		return;
	}

	//zīmē izmaiņas, izņem iepriekšējā daļā neizņemtās čūskas
	for (i = 0; i < server_config.max_players; i++) {
		if (snakes[i].state != 1) {
			continue;
		}
		if (field[new_head[i].x][new_head[i].y] == 'P') {
			field[new_head[i].x][new_head[i].y] = 'H';
			field[snakes[i].points[snakes[i].head_idx].x][snakes[i].points[snakes[i].head_idx].y] = 'T';
			snakes[i].head_idx = get_new_head_idx(i);
			snakes[i].points[snakes[i].head_idx].x = new_head[i].x;
			snakes[i].points[snakes[i].head_idx].y = new_head[i].y;
			snakes[i].string[snakes[i].head_idx] = get_opposite_dir(i);
			if (got_food[i] == 'Y') {
				got_food[i] = 'N';
				players[i].points++;
				if (players[i].points >= game_config.score_limit) {

				}
			} else {
				snakes[i].tail_idx = get_new_tail_idx(i);
				snakes[i].string[snakes[i].tail_idx] = '\0';
				field[snakes[i].points[snakes[i].tail_idx].x][snakes[i].points[snakes[i].tail_idx].y] = ' ';
			}
		} else {
			field[new_head[i].x][new_head[i].y] = ' ';
			snakes[i].state = 2;
		}
	}

	//nodzēš mirušās čūskas
	for (i = 0; i < server_config.max_players; i++) {
		if (snakes[i].state == 2) {
			int d;
			if (snakes[i].head_idx < snakes[i].tail_idx) {
				for (d = snakes[i].head_idx; d <= snakes[i].tail_idx; d++) {
					field[snakes[i].points[d].x][snakes[i].points[d].y] = ' ';
				}	
			}
			else {
				for (d = snakes[i].head_idx; d <=MAX_SCORE_LIMIT + MAX_SNAKE_INITIAL_SIZE; d++) {
					field[snakes[i].points[d].x][snakes[i].points[d].y] = ' ';
				}
				for (d = 0; d <=snakes[i].tail_idx; d++) {
					field[snakes[i].points[d].x][snakes[i].points[d].y] = ' ';
				}
			}
			snakes[i].state = 0;
			players[i].state = 2;
			infof("Players %c snake is dead!", players[i].id);
			send_dead_message(&players[i].addr);
		}
	}

	//atjauno ābolus
	for (i = 0; i < game_config.food_amount; i++) {
		if (field[food[i].x][food[i].y] != (unsigned char) 'A') {
			get_random_food(i);
		}
	}

	//izsūta ziņas
	send_messages();
}

void valid_move(int i)
{
	if ((snakes[i].dir == STATE_MSG_UP_CHAR || snakes[i].dir == STATE_MSG_DOWN_CHAR) && (moves[i] == STATE_MSG_LEFT_CHAR || moves[i] == STATE_MSG_RIGHT_CHAR)) {
		snakes[i].dir = moves[i];
	}
	if ((snakes[i].dir == STATE_MSG_LEFT_CHAR || snakes[i].dir == STATE_MSG_RIGHT_CHAR) && (moves[i] == STATE_MSG_UP_CHAR || moves[i] == STATE_MSG_DOWN_CHAR)) {
		snakes[i].dir = moves[i];
	}
}

char get_opposite_dir(int i)
{
	switch (snakes[i].dir) {
	case STATE_MSG_UP_CHAR:
		return STATE_MSG_DOWN_CHAR;
		break;
	case STATE_MSG_DOWN_CHAR:
		return STATE_MSG_UP_CHAR;
		break;
	case STATE_MSG_LEFT_CHAR:
		return STATE_MSG_RIGHT_CHAR;
		break;
	case STATE_MSG_RIGHT_CHAR:
		return STATE_MSG_LEFT_CHAR;
		break;
	default:
		return ' ';
	}
}

void get_new_head(int i)
{
	if (snakes[i].dir == STATE_MSG_UP_CHAR) {
		new_head[i].y = snakes[i].points[snakes[i].head_idx].y - 1;
		new_head[i].x = snakes[i].points[snakes[i].head_idx].x;
	}
	if (snakes[i].dir == STATE_MSG_DOWN_CHAR) {
		new_head[i].y = snakes[i].points[snakes[i].head_idx].y + 1;
		new_head[i].x = snakes[i].points[snakes[i].head_idx].x;
	}
	if (snakes[i].dir == STATE_MSG_LEFT_CHAR) {
		new_head[i].y = snakes[i].points[snakes[i].head_idx].y;
		new_head[i].x = snakes[i].points[snakes[i].head_idx].x - 1;
	}
	if (snakes[i].dir == STATE_MSG_RIGHT_CHAR) {
		new_head[i].y = snakes[i].points[snakes[i].head_idx].y;
		new_head[i].x = snakes[i].points[snakes[i].head_idx].x + 1;
	}
}

int get_new_head_idx(int i)
{
	if (snakes[i].head_idx == 0) {
		return(MAX_SCORE_LIMIT + MAX_SNAKE_INITIAL_SIZE);
	} else {
		return(snakes[i].head_idx - 1);
	}
}

int get_new_tail_idx(int i)
{
	if (snakes[i].tail_idx == 0) {
		return(MAX_SCORE_LIMIT + MAX_SNAKE_INITIAL_SIZE);
	} else {
		return(snakes[i].tail_idx - 1);
	}
}

void send_messages()
{
	int len;
	int player_count = count_active_players();
	int i;
	message[0] = 'o';
	message[1] = (unsigned char) player_count;
	message[2] = '\0';
	len = 3;
	int player_string_len;
	for (i = 0; i < server_config.max_players; i++) {
		if (players[i].state == 1) {
			player_string_len = get_player_string(i, &message[len]);
			len += player_string_len;
		}
	}
	message[len] = (unsigned char) game_config.food_amount;
	len++;
	message[len] = '\0';
	len++;
	for (i = 0; i < game_config.food_amount; i++) {
		message[len] = food[i].x - 1;
		len++;
		message[len] = food[i].y - 1;
		len++;
	}
	message[len] = '\0';
	len++;
	for (i = 0; i < server_config.max_players; i++) {
		if (players[i].state == 1 || players[i].state == 2) {
			sendto(listener, message, len, 0, (struct sockaddr *) &players[i].addr, sizeof(players[i].addr));
		}
	}
}

int count_active_players()
{
	int count = 0;
	int i;
	for (i = 0; i < server_config.max_players; i++) {
		if (players[i].state == 1) {
			count++;
		}
	}
	return count;
}

int get_player_string(int i, char * buffer)
{
	int code_len;
	int k = 3;
	buffer[0] = players[i].id;
	buffer[1] = (unsigned char) (snakes[i].points[snakes[i].head_idx].x - 1);
	buffer[2] = (unsigned char) (snakes[i].points[snakes[i].head_idx].y - 1);
#ifdef CONFIG_COMPRESSED_STATE
	buffer[k] = (unsigned char) (snakes[i].size - 1);
	k++;
	code_len = get_snake_coded(i, &buffer[k]);
#else
	code_len = get_snake_original(i, &buffer[k]);
#endif
	buffer[k + code_len] = '\0';
	return code_len + k + 1;
}

int get_snake_original(int i, char * buffer)
{
	int k = 0;
	int c;
	if (snakes[i].head_idx < snakes[i].tail_idx) {
		for (c = snakes[i].head_idx; c < snakes[i].tail_idx; c++) {
			buffer[k] = snakes[i].string[c];
			k++;
		}
	} else {
		for (c = snakes[i].head_idx; c < MAX_SCORE_LIMIT + MAX_SNAKE_INITIAL_SIZE + 1; c++) {
			buffer[k] = snakes[i].string[c];
			k++;


		}
		for (c = 0; c < snakes[i].tail_idx; c++) {
			buffer[k] = snakes[i].string[c];
			k++;
		}
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


