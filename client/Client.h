#ifndef CLIENT_H
#define	CLIENT_H

#include "../common/Structures.h"

extern struct Client client_config;
extern unsigned char field[MAX_GAME_WIDTH*MAX_GAME_HEIGHT];
void parse_config();
int register_into_server();
void disconnect_from_server();
void game_loop();

/* Messages */
size_t create_register_message(char *msg, const char *username);
size_t create_start_message(char *msg, unsigned char id);
size_t create_move_message(char *msg, char move, unsigned char id);
size_t create_leave_message(char *msg, unsigned char id);

int decode_accept_message(const char *msg, ssize_t len);
void decode_message(const char *msg, ssize_t len);

void send_player_action(int input);

/* Graphics */
enum bash_color {
	DEFAULT_COLOR = 0,
	RED_COLOR,
	GREEN_COLOR,
	YELLOW_COLOR,
	BLUE_COLOR,
	MARGENTA_COLOR,
	CYAN_COLOR
};
extern unsigned char foreground[];
extern unsigned char background[];

void bash_position_cursor(int x, int y);
void bash_clear_screen();
void bash_clear_line();
void bash_set_color(unsigned char color);
void bash_save_cursor_pos();
void bash_restore_cursor_pos();
void bash_set_window_size(int x, int y);
void bash_hide_cursor();


#endif	/* CLIENT_H */

