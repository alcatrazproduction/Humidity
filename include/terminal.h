/*****************************************************************************
 *  (c) FONDAREX SA
 *  $Author:       Yves Huguenin
 *  $Title:        terminal.h
 *  $Create Date:  31.10.2014
 * $Vers:         1.0a1
 *
 *  1.0a1:
 *
 * **************************************************************************/
#ifndef __FONDAREX_TERMINAL__
#define __FONDAREX_TERMINAL__


class termial
{
private:
	int 				sockfd;
	int					newsockfd;
	struct sockaddr_in	serv_addr;
	struct sockaddr_in	cli_addr;
};
#endif