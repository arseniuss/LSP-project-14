/*
 * File:   Defaults.h
 * Author: arseniuss
	   Ilze Dzene-Vanaga
 *
 */

#ifndef DEFAULTS_H
#define	DEFAULTS_H

#define MAX_PLAYER_COUNT		4
#define MAX_MESSAGE_SIZE		256
#define MAX_USERNAME_LEN		15
#define MAX_GAME_WIDTH			120
#define MAX_GAME_HEIGHT			80
#define MAX_SNAKE_LENGTH		256
#define MAX_FOOD_AMOUNT			4
#define MAX_SNAKE_INITIAL_SIZE		6
#define MAX_SCORE_LIMIT			15

#define DEFAULT_GAME_WIDTH		80
#define DEFAULT_GAME_HEIGHT		20
#define DEFAULT_GAME_SPEED		5
#define DEFAULT_FOOD_AMOUNT		2
#define DEFAULT_SCORE_LIMIT		15
#define DEFAULT_TIME_LIMIT		2
#define DEFAULT_PLAYER_COUNT		4

#define DEFAULT_BLANK_CHAR		' '
#define DEFAULT_SNAKE_CHAR		'S'

#define DEFAULT_SNAKE_INITIAL_SIZE	3

#define DEFAULT_SERVER_ADDRESS		"127.0.0.1"
#define DEFAULT_SERVER_PORT_NO		65342

#define DEFAULT_TRIES			5
#define DEFAULT_SLEEP			1 /* second */

#define PLAYER_STATE_INITIAL		0
#define PLAYER_STATE_ACTIVE		1
/* States below PLAYER_STATE_DEAD should be in- and pre- game states */
#define PLAYER_STATE_DEAD		2
#define PLAYER_STATE_END		3
#define PLAYER_STATE_LEFT		4

#endif	/* DEFAULTS_H */
