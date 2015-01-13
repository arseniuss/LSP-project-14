/*
 * File:   client/graphics.c
 * Author: Armands Skolmeisters
 *
 * Shell graphics
 */

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "Client.h"

unsigned char foreground[] = {
	[DEFAULT_COLOR] = 39,
	[BLACK_COLOR] = 30,
	[RED_COLOR] = 31,
	[GREEN_COLOR] = 32,
	[YELLOW_COLOR] = 33,
	[BLUE_COLOR] = 34,
	[MARGENTA_COLOR] = 35,
	[CYAN_COLOR] = 36
};

unsigned char background[] = {
	[DEFAULT_COLOR] = 49,
	[BLACK_COLOR] = 40,
	[RED_COLOR] = 41,
	[GREEN_COLOR] = 42,
	[YELLOW_COLOR] = 43,
	[BLUE_COLOR] = 44,
	[MARGENTA_COLOR] = 45,
	[CYAN_COLOR] = 46
};

void bash_position_cursor(int x, int y)
{
	system("PS1=\"\"");
	printf("\033[%d;%dH", y, x);
}

void bash_clear_screen()
{
	printf("\033[2J");
}

void bash_clear_line()
{
	printf("\r\033[K");
}

void bash_set_color(unsigned char color)
{
	printf("\e[%dm", color);
}

void bash_save_cursor_pos()
{
	printf("\033[s");
}

void bash_restore_cursor_pos()
{
	printf("\033[u");
}

void bash_set_window_size(int x, int y)
{
	printf("\033[8;%d;%dt", y + 4, x + 1);
}

void bash_hide_cursor()
{
	printf("\e[?25l");
}
