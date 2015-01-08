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

enum GameStates {
	STATE_INITIAL = 0,
	STATE_GAME,
};
int game_state;
int listener;

struct Player players[MAX_PLAYER_COUNT];

char send_buffer[MAX_MESSAGE_SIZE];

void handle_register_message(const char *msg, struct sockaddr_in *addr)
{
	int i;

	if (msg[0] != 'r' && msg[1] != '\0') {
		debugf("Wrong REGISTER message format!\n");
		return;
	}

	if (game_state != STATE_INITIAL) {
		debugf("Cannot REGISTER: game is already started\n");
		goto send_no_msg;
	}

	for (i = 0; i < server_config.max_players; ++i) {
		if (!strcmp(players[i].username, &msg[2])) {
			debugf("User '%s' already exists!\n", players[i].username);
			goto send_no_msg;
		}
	}

	for (i = 0; i < server_config.max_players; ++i) {
		if (players[i].username[0] == '\0') {
			players[i].id = random_player_id();
			memcpy(&players[i].addr, addr, sizeof(*addr));
			strncpy(players[i].username, &msg[2], MAX_USERNAME_LEN);

			infof("User '%s' from %s saved as player no. %d with ID '%c'",
				players[i].username, inet_ntoa(players[i].addr.sin_addr),
				0, players[i].id);

			i = create_yes_message(send_buffer, players[i].id);
			sendto(listener, send_buffer, i, 0,
				(struct sockaddr *) addr, sizeof(*addr));

			return;
		}
	}

send_no_msg:
	sendto(listener, no_message, no_message_len, 0,
		(struct sockaddr *) addr, sizeof(*addr));
}

void handle_message(const char *msg, size_t len, struct sockaddr_in *addr)
{
	int i;

	if (len <= 0)
		return;

#ifdef DEBUG
	debugf("Message (%ld): ", len);
	for (i = 0; i < len; ++i) {
		if (msg[i] == '\0')
			printf("\\0");
		else
			putchar(msg[i]);
	}
	printf("\n");
#endif

	switch (msg[0]) {
	case 'r':
		handle_register_message(msg, addr);
		break;
	case 's':
		for (i = 0; i < server_config.max_players; ++i) {
			if (players[i].addr.sin_addr.s_addr != addr->sin_addr.s_addr)
				continue;
			/* TODO: Extension to protocol */
			if (len > 2) {
				if (strcmp(players[i].username, &msg[2]))
					continue;
			}
			if (game_state == STATE_INITIAL) {
				infof("Game started!");
				game_state = STATE_GAME;
			}
			break;
		}
		break;
	}
}

void send_messages()
{
	switch (game_state) {
	case STATE_GAME:
		break;
	}
}

int main_loop()
{
	int nbytes;
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
	socklen_t addrlen;
	char msg[MAX_MESSAGE_SIZE];

	bzero(&players, sizeof(players));

	listener = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (listener < 0)
		error("Error while opening socket");

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(server_config.port_no);

	if (bind(listener, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		error("ERRROR on binding");
	}

	addrlen = sizeof(cli_addr);

	game_state = STATE_INITIAL;

	infof("Waiting for incoming players");

	while (1) {
		if ((nbytes = recvfrom(listener, msg, MAX_MESSAGE_SIZE,
			MSG_DONTWAIT, (struct sockaddr *) &cli_addr,
			&addrlen)) > 0) {

			handle_message(msg, nbytes, &cli_addr);
		}

		send_messages();
	}

	close(listener);

	return 0;
}
