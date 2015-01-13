/*
 * File:   client/game.c
 * Author: Armands Skolmeisters
 *
 * Game logic and staff
 */

#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>

#include "Client.h"
#include "../common/Defs.h"
#include "../common/Structures.h"

int sender;
struct Client client_config;
char msg[MAX_MESSAGE_SIZE];
size_t msg_len;
ssize_t resp_len;

struct Snake snake[MAX_PLAYER_COUNT];
int snake_count = 0;
struct Point food[MAX_FOOD_AMOUNT];

void connect_to_server()
{
	int tries, resp;
	socklen_t slen = sizeof(client_config.addr);

	printf("Savienojas ar serveri %s:%d ", client_config.server_ip,
		client_config.port_no);

	if ((sender = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		error("Error while opening socket");
	}

	fcntl(sender, F_SETFL, O_NONBLOCK);

	putchar('.');

	memset(&client_config.addr, 0, sizeof(client_config.addr));
	client_config.addr.sin_family = AF_INET;
	client_config.addr.sin_port = htons(client_config.port_no);

	if (inet_aton(client_config.server_ip, &client_config.addr.sin_addr) == 0) {
		error("Wrong server IP address!");
	}

	putchar('.');

	msg_len = create_register_message(msg, client_config.username);

	if (sendto(sender, msg, msg_len, 0, (struct sockaddr *) &client_config.addr,
		slen) == -1) {
		error("Error while sending REGISTER message");
	}

	tries = 0;
	do {
		putchar('.');

		if ((resp_len = recvfrom(sender, msg, MAX_MESSAGE_SIZE, 0,
			(struct sockaddr *) &client_config.addr, &slen)) == -1) {
			if (errno != EAGAIN)
				error("Error while receiving ACCEPT message");
		}

		if (resp_len > 0) {
			break;
		}

		tries++;
		fflush(stdout);
		sleep(DEFAULT_SLEEP);
	} while (tries < DEFAULT_TRIES);

	if (resp_len <= 0) {
		bash_set_color(foreground[RED_COLOR]);
		fflush(stdout);
		error("\nNevar piereģistrēties serverī %s:%d",
			client_config.server_ip, client_config.port_no);
	}

	if ((resp = decode_accept_message(msg, resp_len)) < 0) {
		error("Error while decoding ACCEPT message");
	}

	if (resp == 0) {
		printf(" aizņemts!");
	} else {
		printf(" pieslēdzies!");
	}
}

void send_player_input(int input)
{
	switch (input) {
	case 'w':
	case 'd':
	case 'a':
	case 's':
		/* In case of caps lock */
	case 'W':
	case 'D':
	case 'A':
	case 'S':
		create_move_message(msg, input, client_config.id);
		break;
	case 'b':
	case 'B':
		create_start_message(msg, client_config.id);
		break;
	case 'q':
	case 'Q':
		create_leave_message(msg, client_config.id);
		break;
	}
}

void game_draw()
{
	int i, j;

	for (i = 0; i < client_config.height; ++i)
		for (j = 0; j < client_config.width; ++j) {

		}
}

void game_loop()
{
	int player_input;
	socklen_t slen = sizeof(client_config.addr);

	/* Set input to nonblock so we gan process everthing else */
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

	bash_set_window_size(client_config.width, client_config.height);

	do {
		if ((resp_len = recvfrom(sender, msg, MAX_MESSAGE_SIZE, 0,
			(struct sockaddr *) &client_config.addr, &slen)) == -1) {
			if (errno != EAGAIN)
				error("Error while receving game updates");
		}

		if (resp_len > 0) {
			decode_message(msg, resp_len);
		}

		if ((player_input = fgetc(stdin)) != EOF) {
			send_player_input(player_input);

			switch (player_input) {
			case 'Q':
			case 'q':
				client_config.state = PLAYER_STATE_LEFT;
				break;
			}
		}
	} while (client_config.state == PLAYER_STATE_ACTIVE);
}
