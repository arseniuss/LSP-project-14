#ifndef CLIENT_H
#define	CLIENT_H

#include "../common/Structures.h"

extern struct Client client_config;

struct ClientSnake {
	int len;
	unsigned char id;
	struct Point points[MAX_SNAKE_LENGTH];
};
extern struct ClientSnake snake[MAX_PLAYER_COUNT];
extern int snake_count;
extern struct Point food[MAX_FOOD_AMOUNT];

void parse_config();
int connect_to_server();
void disconnect_from_server();
void game_loop();

/* Messages */
size_t create_register_message(char *msg, const char *username);
size_t create_start_message(char *msg, unsigned char id);
size_t create_move_message(char *msg, char move, unsigned char id);
size_t create_leave_message(char *msg, unsigned char id);

int decode_accept_message(const char *msg, ssize_t len);
void decode_message(const char *msg, ssize_t len);

/* Graphics */
enum bash_color {
	DEFAULT_COLOR = 0,
	BLACK_COLOR,
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


#endif	/* CLIENT_H */

