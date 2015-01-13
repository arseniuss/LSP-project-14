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
#include <stdlib.h>

#include "Client.h"
#include "../common/Defs.h"
#include "../common/Structures.h"

int sender;
struct Client client_config;
char msg[MAX_MESSAGE_SIZE];
size_t msg_len;
ssize_t resp_len;

unsigned char field[MAX_GAME_WIDTH * MAX_GAME_HEIGHT];

int register_into_server()
{
	int tries, resp, ret;
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
		printf("\nNevar piereģistrēties serverī %s:%d\n",
			client_config.server_ip, client_config.port_no);
		bash_set_color(foreground[DEFAULT_COLOR]);
		return 0;
	}

	if ((resp = decode_accept_message(msg, resp_len)) < 0) {
		error("Error while decoding ACCEPT message");
	}

	if (resp == 0) {
		printf(" aizņemts!\n");
		ret = 0;
	} else {
		printf(" pieslēdzies!\n");
		ret = 1;
	}

	fflush(stdout);

	return ret;
}

void disconnect_from_server()
{
	send_player_action('q');
	close(sender);
}

void send_player_action(int input)
{
	socklen_t slen = sizeof(client_config.addr);

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
		msg_len = create_move_message(msg, input, client_config.id);
		break;
	case 'b':
	case 'B':
		msg_len = create_start_message(msg, client_config.id);
		break;
	case 'q':
	case 'Q':
		msg_len = create_leave_message(msg, client_config.id);
		break;
	default:
		return;
	}

	if (sendto(sender, msg, msg_len, 0, (struct sockaddr *) &client_config.addr,
		slen) == -1) {
		error("Error while sending user input message");
	}
}

void game_draw()
{
	int i, j;
	unsigned char ch;
#ifndef DEBUG
	//bash_clear_screen();
	bash_position_cursor(0, 0);
	for (i = 0; i < client_config.height; ++i) {
		for (j = 0; j < client_config.width; ++j) {
			ch = field[j + i * client_config.width];

			switch (ch) {
			case ' ':
				bash_set_color(foreground[DEFAULT_COLOR]);
				putchar(' ');
				break;
			default:
				if (ch >= 'f') {
					bash_set_color(foreground[DEFAULT_COLOR + ch - 'f' + 1]);
					printf("🍒");
					bash_set_color(foreground[DEFAULT_COLOR]);
				} else {
					bash_set_color(foreground[DEFAULT_COLOR + ch + 1]);
					printf("█");
					bash_set_color(foreground[DEFAULT_COLOR]);
				}
				break;
			}
		}
		putchar('\n');
	}
	putchar('\n');
#endif
}

void game_loop()
{
	int player_input;
	socklen_t slen = sizeof(client_config.addr);

	printf("Nospiediet '%c', lai sāktu spēli!\n", 'b');

	bash_set_window_size(client_config.width, client_config.height);
	bash_hide_cursor();
	client_config.state = PLAYER_STATE_INITIAL;

	memset(field, DEFAULT_BLANK_CHAR, sizeof(field));
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
			send_player_action(player_input);

			switch (player_input) {
			case 'Q':
			case 'q':
				client_config.state = PLAYER_STATE_LEFT;
				break;
			}

		}

		/* Draw only when in game */
		if (client_config.state == PLAYER_STATE_ACTIVE)
			game_draw();
	} while (client_config.state < PLAYER_STATE_DEAD);

	switch (client_config.state) {
	case PLAYER_STATE_DEAD:
		printf("Jūs nomirāt!\n");
		break;
	case PLAYER_STATE_END:
		if (client_config.winner == client_config.id)
			printf("Jūs uzvarējāt!\n");
		break;
	}
}
