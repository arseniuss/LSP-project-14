/*
 * File:   client/mainc.c
 * Author: Armands Skolmeisters
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <signal.h>

#include "Client.h"
#include "../common/Defaults.h"
#include "../common/Defs.h"
#include "../common/Structures.h"

struct termios ttystate, ttysave;

void show_title()
{
	int i;
	char *title = " Multiplayer snake ";
	struct winsize w;

	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	putchar('\n');
	for (i = 0; i < (w.ws_col - strlen(title)) / 2; ++i)
		putchar(' ');
	bash_set_color(foreground[GREEN_COLOR]);
	puts(title);
	bash_set_color(foreground[DEFAULT_COLOR]);
	printf("\n");
}

void input_username()
{
	int i;
	bool username_good = false;

	bash_save_cursor_pos();
	do {
		bash_set_color(foreground[DEFAULT_COLOR]);

		printf("Lietotājvārds: ");
		scanf("%" xstringify(MAX_USERNAME_LEN) "s", client_config.username);

		/* Clear input buffer*/
		while (getchar() != '\n');

		username_good = true;

		for (i = 0; i < strlen(client_config.username); ++i) {
			if (!isalpha(client_config.username[i])) {
				username_good = false;
				break;
			}
		}

		bash_restore_cursor_pos();
		bash_set_color(foreground[RED_COLOR]);
		if (!username_good) {
			bash_clear_line();
			printf("Neatļautas zīmes lietotājvārdā! ");
			continue;
		}

		if (strlen(client_config.username) > MAX_USERNAME_LEN) {
			bash_clear_line();
			printf("Pārāk garš lietotājvārds! ");
			continue;
		}


		if (strlen(client_config.username) == MAX_USERNAME_LEN) {
			bash_clear_line();
			bash_set_color(foreground[DEFAULT_COLOR]);
			printf("Tiks izmantots lietotājvārds '%s'. Der? J/N ",
				client_config.username);
			i = getchar();

			if (i == 'j' || i == 'J')
				break;
			username_good = false;
			bash_restore_cursor_pos();
			bash_clear_line();
		}
	} while (username_good == false);

	bash_clear_line();
	bash_set_color(foreground[DEFAULT_COLOR]);
	printf("Lietotājvārds: %s pieņemts\n", client_config.username);
}

int repeat_game()
{
	int i;

	do {
		printf("Vai atkārtot spēli? J/N ");

		while ((i = getchar()) == EOF);
		printf("%c\n", i);

	} while (i != 'j' && i != 'n' && i != 'J' && i != 'N');



	return i == 'j' || i == 'J' ? 1 : 0;
}

void setup_char_input()
{

	/* Set non-blocking and non-waiting state for stdin */
	tcgetattr(STDIN_FILENO, &ttystate);

	memcpy(&ttysave, &ttystate, sizeof(struct termios));

	ttystate.c_lflag &= ~(ICANON | ECHO);
	ttystate.c_cc[VMIN] = 0;
	ttystate.c_cc[VTIME] = 0;

	tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

void unset_char_input()
{

	/* Restore old stdin settings */
	tcsetattr(STDIN_FILENO, TCSANOW, &ttysave);
}

void exit_signal(int i)
{
	if (client_config.sockfd) {
		send_player_action('Q');
		disconnect_from_server();
	}
	exit(0);
}

int main(int argc, char** argv)
{
	parse_config();
	show_title();
	input_username();

	signal(SIGINT, exit_signal);
	setup_char_input();
	if (register_into_server())
		do {
			game_loop();

			if (!repeat_game()) {
				send_player_action('Q');
				break;
			}
		} while (1);
	disconnect_from_server();
	unset_char_input();

	return 0;
}
