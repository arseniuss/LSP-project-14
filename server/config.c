/*
 * File:   server/config.c
 * Author: Armands Skolmeisters
 *
 * Configuration parsing for server
 */

/**
 * Using Glib 2.0
 */

#include <string.h>
#include <glib-2.0/glib.h>
#include <stdlib.h>

#include "../common/Structures.h"
#include "../common/Defaults.h"
#include "../common/Defs.h"

struct Server server_config;
struct Game game_config;

GKeyFile *key_file;

char *player_ids = "~!@#$%^&*()_+`1234567890-=";

void shuffle_ids()
{
	int i, j;
	unsigned char c;
	srand(time(NULL));

	for (i = 0; i < strlen(player_ids); ++i) {
		j = rand() % strlen(player_ids);
		c = player_ids[i];
		player_ids[i] = player_ids[j];
		player_ids[j] = c;
	}
}

char random_player_id()
{
	static int next;

	if (next < 0)
		next = 0;
	if (next > strlen(player_ids))
		next = 0;

	return player_ids[next++];
}

static void get_key(const char *group, const char *key, unsigned char *field,
	int min, int max)
{
	int i;

	if ((i = g_key_file_get_integer(key_file, group, key, NULL)) != 0) {
		if ((min == 0 && max == 0) || (i >= min && i <= max)) {
			debugf("Setting %s.%s to %d\n", group, key, i);
			*field = (unsigned char) i;
			return;
		}

		infof("Wrong %s field in file server.ini!", key);
		if (min != 0)
			infof("Range [%d-%d]\n", min, max);
		error("ERROR while parsing server.ini");
	}
}

void parse_config()
{
	char *cptmp;
	int itmp;

	key_file = g_key_file_new();

	/* Setup default configuration */
	server_config.server_ip = DEFAULT_SERVER_ADDRESS;
	server_config.port_no = DEFAULT_SERVER_PORT_NO;
	server_config.max_players = DEFAULT_PLAYER_COUNT;

	game_config.field_width = DEFAULT_GAME_WIDTH;
	game_config.field_height = DEFAULT_GAME_HEIGHT;
	game_config.snake_speed = DEFAULT_GAME_SPEED;
	game_config.initial_size = DEFAULT_SNAKE_INITIAL_SIZE;
	game_config.food_amount = DEFAULT_FOOD_AMOUNT;
	game_config.time_limit = DEFAULT_TIME_LIMIT;
	game_config.score_limit = DEFAULT_SCORE_LIMIT;

	if (g_key_file_load_from_file(key_file, "server.ini",
		G_KEY_FILE_NONE, NULL) != TRUE) {
		infof("Could not open server.ini file! Using default configuration");
		goto out;
	}

	if ((cptmp = g_key_file_get_string(key_file, "server",
		"SERVER_IP", NULL)) != NULL) {
		debugf("Setting server.SERVER_IP to %s\n", cptmp);
		server_config.server_ip = cptmp;
	}

	if ((itmp = g_key_file_get_integer(key_file, "server",
		"SERVER_PORT", NULL)) != 0) {
		debugf("Setting server.SERVER_PORT to %d\n", itmp);
		server_config.port_no = itmp;
	}

	get_key("server", "MAX_PLAYERS", &server_config.max_players, 0, 4);

	get_key("game", "FIELD_WIDTH", &game_config.field_width, 20, 120);
	get_key("game", "FIELD_HEIGHT", &game_config.field_width, 20, 80);
	get_key("game", "SNAKE_SPEED", &game_config.snake_speed, 2, 10);
	get_key("game", "SNAKE_SIZE", &game_config.initial_size, 1, 6);
	get_key("game", "SNAKE_FOOD_AMOUNT", &game_config.food_amount, 1, 4);
	get_key("game", "GAME_TIME_LIMIT", &game_config.time_limit, 0, 10);
	get_key("game", "GAME_SCORE_LIMIT", &game_config.score_limit, 5, 15);

out:
	g_key_file_free(key_file);
}
