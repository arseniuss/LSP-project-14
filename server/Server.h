/*
 * File:   Server.h
 * Author: arseniuss
 *
 * Server header
 */

#ifndef SERVER_H
#define	SERVER_H

#include "../common/Structures.h"

void parse_config();
int main_loop();

extern struct Server server_config;
extern struct Game game_config;
enum GameStates {
	STATE_INITIAL = 0,
	STATE_GAME,
};
extern int game_state;
extern int listener;
extern struct Point snakes[MAX_PLAYER_COUNT][DEFAULT_SCORE_LIMIT + 3];
extern struct Player players[MAX_PLAYER_COUNT];
extern char send_buffer[MAX_MESSAGE_SIZE];
extern int do_move;

/* Thread functions */
void timer_thread_function();
void listener_thread_function();

/* Messages */
//int message_parse(const char *msg, ...);
void parse_message(size_t len, char *msg);
size_t create_yes_message(char *msg, unsigned char id);
size_t create_no_message(char *msg);
size_t create_end_message(char *msg, unsigned char id);
size_t create_dead_message(char *msg);

void handle_register_message(const char *msg, struct sockaddr_in *addr);
void send_message(struct sockaddr_in *addr, size_t len, char * msg);
void send_no_message(struct sockaddr_in *addr);
void handle_message(const char *msg, size_t len, struct sockaddr_in *addr);
void send_messages();
int get_player_from_addr_id(struct sockaddr_in *addr, unsigned char id);
int is_valid_move(unsigned char m);


/* Player IDs */
void shuffle_ids();
char random_player_id();

/* Gameplay */
void remove_player(int i);
void player_move(int i, unsigned char m);
void move_all();


#endif	/* SERVER_H */
