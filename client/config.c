/*
 * File:   client/config.c
 * Author: Armands Skolmeisters
 *
 * Configuration parsing for client
 */

/**
 * Using Glib 2.0
 */

#include <stdlib.h>
#include <glib-2.0/glib.h>

#include "../common/Defs.h"
#include "Client.h"

GKeyFile *key_file;

void parse_config()
{
	char *cptmp;
	int itmp;

	key_file = g_key_file_new();

	client_config.server_ip = DEFAULT_SERVER_ADDRESS;
	client_config.port_no = DEFAULT_SERVER_PORT_NO;

	if (g_key_file_load_from_file(key_file, "server.ini",
		G_KEY_FILE_NONE, NULL) != TRUE) {
		infof("Could not open server.ini file! Using default configuration");
		goto out;
	}

	if ((cptmp = g_key_file_get_string(key_file, "server",
		"SERVER_IP", NULL)) != NULL) {
		debugf("Setting server.SERVER_IP to %s\n", cptmp);
		client_config.server_ip = cptmp;
	}

	if ((itmp = g_key_file_get_integer(key_file, "server",
		"SERVER_PORT", NULL)) != 0) {
		debugf("Setting server.SERVER_PORT to %d\n", itmp);
		client_config.port_no = itmp;
	}

out:
	g_key_file_free(key_file);
}
