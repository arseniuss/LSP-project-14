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


/* Messages */
int message_parse(const char *msg, ...);

#define no_message_len			4
extern const char *no_message;
size_t create_yes_message(char *buffer, unsigned char id);

/* Player IDs */
void shuffle_ids();
char random_player_id();

#endif	/* SERVER_H */

