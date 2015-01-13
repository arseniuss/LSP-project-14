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
#include "../common/Messages.h"
#include "Server.h"

void listener_thread_function()
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

	infof("Staring server at %s port %d", server_config.server_ip,
		server_config.port_no);
	infof("Waiting for incoming players");

	while (1) {
		if ((nbytes = recvfrom(listener, msg, MAX_MESSAGE_SIZE,
			MSG_DONTWAIT, (struct sockaddr *) &cli_addr,
			&addrlen)) > 0) {

			handle_message(msg, nbytes, &cli_addr);
		}

		//send_messages();
	}

	close(listener);

	return;
}

void handle_register_message(const char *msg, struct sockaddr_in *addr)
{
	int i;
	size_t len;

	if (msg[0] != 'r' && msg[1] != '\0') {
		debugf("Wrong REGISTER message format!\n");
		return;
	}

	if (game_state != STATE_INITIAL) {
		debugf("Cannot REGISTER: game is already started\n");
		send_no_message(addr);
		return;
	}

	for (i = 0; i < server_config.max_players; ++i) {
		if (!strcmp(players[i].username, &msg[2])) {
			infof("User '%s' already exists!\n", players[i].username);
			send_no_message(addr);
			return;
		}
		if (players[i].username[0] == '\0') {
			players[i].id = random_player_id();
			memcpy(&players[i].addr, addr, sizeof(*addr));
			strncpy(players[i].username, &msg[2], MAX_USERNAME_LEN);
			players[i].state = 1;
			players[i].points = 0;

			infof("User '%s' from %s saved as player no. %d with ID '%c'",
				players[i].username, inet_ntoa(players[i].addr.sin_addr),
				i, players[i].id);

			len = create_yes_message(send_buffer, players[i].id);
			send_message(addr, len, send_buffer);

			return;
		}
	}
}

void send_message(struct sockaddr_in *addr, size_t len, char * msg)
{
	sendto(listener, msg, len, 0, (struct sockaddr *) addr, sizeof(*addr));
}

void send_no_message(struct sockaddr_in *addr)
{
	size_t len = create_no_message(send_buffer);
	send_message(addr, len, send_buffer);
}

void handle_message(const char *msg, size_t len, struct sockaddr_in *addr)
{
	if (len <= 0)
		return;

	debug_message(msg, len);

	switch (msg[0]) {
	case 'r':
		handle_register_message(msg, addr);
		break;
	case 's':
		if (len > 2) {
			int i = get_player_from_addr_id(addr, msg[2]);
			if (i != -1) {
				if (game_state == STATE_INITIAL) {
					infof("Game started!");
					game_state = STATE_GAME;
				}
			} else {
				infof("Incorrect player!");
			}
		}
		break;
	case 'l':
		if (len > 2) {
			int i = get_player_from_addr_id(addr, msg[2]);
			if (i != -1) {
				remove_player(i);
			}
		}
		break;
	case 'm':
		if (len > 4) {
			int i = get_player_from_addr_id(addr, msg[4]);
			if (i != -1 && is_valid_move(msg[2])) {
				if (game_state == STATE_GAME) {
					player_move(i, msg[2]);
				}
			}
		}
	}
}

int get_player_from_addr_id(struct sockaddr_in *addr, unsigned char id)
{
	int i;
	for (i = 0; i < server_config.max_players; ++i) {
		if (players[i].addr.sin_addr.s_addr != addr->sin_addr.s_addr) {
			continue;
		}
		if (players[i].id != id) {
			continue;
		}
		return i;
	}
	return -1;
}

int is_valid_move(char m)
{
	if (m == STATE_MSG_UP_CHAR || m == STATE_MSG_DOWN_CHAR || m == STATE_MSG_LEFT_CHAR || m == STATE_MSG_RIGHT_CHAR) {
		return 1;
	}
	return 0;
}
