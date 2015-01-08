/*
 * File:   main.c
 * Author: Armands Skolmeisters
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "../common/Defs.h"
#include "Server.h"

int main(int argc, char** argv)
{
	parse_config();

	infof("Staring server at %s port %d", server_config.server_ip,
		server_config.port_no);

	if (main_loop()) {
		error("ERROR in initial state");
	}

	return(EXIT_SUCCESS);
}
