/*
 * File:   Structures.h
 * Author: arseniuss
 */

#ifndef STRUCTURES_H
#define	STRUCTURES_H

#include <netinet/in.h>

#include "../common/Defaults.h"

struct Point {
	unsigned char x;
	unsigned char y;
};

struct Game {
	unsigned char field_width; /* 20 - 120 */
	unsigned char field_height; /* 20 - 80 */
	unsigned char snake_speed; /* 2 - 10 blocks/sec */
	unsigned char initial_size; /* 1 - 6 */
	unsigned char food_amount; /* 1 - 4 */
	unsigned char time_limit; /* 0 - 10 minutes */
	unsigned char score_limit; /* 5 - 15 points */
};

/**
 * Server configuration, used in server
 */
struct Server {
	char *server_ip;
	int port_no;
	unsigned char max_players; /* 1- 4 */
};

/**
 * Client configuration, used in client
 */
struct Client {
	unsigned char id;
	unsigned char winner;
	unsigned char state; //0 - left, 1 - active, 2 - dead
	char username[MAX_USERNAME_LEN];


	int width;
	int height;

	int sockfd;
	char *server_ip;
	int port_no;
	struct sockaddr_in addr;
};

/**
 * Player description, used in server
 */
struct Player {
	int sockfd;
	struct sockaddr_in addr;

	unsigned char id;
	char username[MAX_USERNAME_LEN];
	unsigned char state; //0 - left, 1 - active, 2 - dead
	unsigned char points;
};

struct Snake {
	struct Point points[MAX_SCORE_LIMIT + MAX_SNAKE_INITIAL_SIZE + 1];
	char string[MAX_SCORE_LIMIT + MAX_SNAKE_INITIAL_SIZE + 1];
	unsigned char state; //0 - dead, 1 - active, 2 - is dying
	unsigned char size;
	unsigned char head_idx;
	unsigned char tail_idx;
	char dir;
};

#endif	/* STRUCTURES_H */
