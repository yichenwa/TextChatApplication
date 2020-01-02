/**
 * @yichenwa_assignment1
 * @author  Yichen Wang <yichenwa@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 */
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <vector>
#include <sstream>
#include <iomanip>

#include <sys/types.h>
#include <sys/socket.h>

#include "../include/global.h"
#include "../include/logger.h"


#include "../include/server.h"
#include "../include/client.h"

using namespace std;


/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */


int main(int argc, char **argv)
{
	
	/*Init. Logger*/
	cse4589_init_log(argv[2]);

	/*Get PORTnumber and IP address--YIC*/
	int portnumber=atoi(argv[2]);

	/* Clear LOGFILE*/
	fclose(fopen(LOGFILE, "w"));

	/*Start Here*/
	
	if (strcmp(argv[1],"s")==0){
		setup_Server(portnumber);
	}

	if (strcmp(argv[1],"c")==0){
		setup_Client(portnumber);
		
	}

	return 0;
	//exit(0);

}

