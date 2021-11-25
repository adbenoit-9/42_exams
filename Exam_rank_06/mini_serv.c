/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mini_serv.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/22 16:58:24 by adbenoit          #+#    #+#             */
/*   Updated: 2021/11/25 18:17:05 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/select.h>

void    exitError(char *str)
{
	write(STDERR_FILENO, str, strlen(str));
	write(STDERR_FILENO, "\n", 1);
	exit(1);
}

int extract_message(char **buf, char **msg)
{
	char	*newbuf;
	int	i;

	*msg = 0;
	if (*buf == 0)
		return (0);
	i = 0;
	while ((*buf)[i])
	{
		if ((*buf)[i] == '\n')
		{
			newbuf = calloc(1, sizeof(*newbuf) * (strlen(*buf + i + 1) + 1));
			if (newbuf == 0)
				return (-1);
			strcpy(newbuf, *buf + i + 1);
			*msg = *buf;
			(*msg)[i + 1] = 0;
			*buf = newbuf;
			return (1);
		}
		i++;
	}
	return (0);
}

void	handle_socket(int sockfd, struct sockaddr_in servaddr)
{
	int					connfd, retval, client_id;
	fd_set				rfd, wfd;
	socklen_t			len;
	struct sockaddr_in	cli[4096];
	char				str[80];
	
	client_id = 0;
	while (true)
	{
		/* waits for a connection */
		retval = select(2, &rfd, &wfd, NULL, NULL);
		
		/* error case */
		if (retval == -1)
			exitError("Fatal Error");
		/* connexion found */
		else if (retval > 0)
		{
			len = sizeof(struct sockaddr_in);
			connfd = accept(sockfd, (struct sockaddr *)&cli[client_id], &len);
			if (connfd < 0)
				exitError("Fatal Error");
			/*
			** 1 - check if the client is already connected.
			** 2 - check if he is trying to read or send a message.
			** 3 - process the action (request or response).
			** 4 - if the client try to send a message, send it to all clients connected.

			** How to reconize the action of a client ?
			** How to know when a client diconnects ?
			** How to reconize a client ? connfd ?
			** answers in struct sockaddr nof the client ?
			** NB: create an array of clients with the connfd and the struct sockaddr.
			*/ 
			
			sprintf(str, "server: client %d just arrived\n", client_id);
			write(STDERR_FILENO, str, strlen(str));
			++client_id;
			
			// FD_SET(connfd, &rfd); // fd that you will receive will already be set ?
			// FD_SET(connfd, &wfd); // fd that you will receive will already be set ?
		}
	}
}

int main (int ac, char **av)
{
	int					port, sockfd;
	struct sockaddr_in	servaddr;

	if (ac < 2)
		exitError("Wrong number of arguments");
	port = atoi(av[1]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	bzero(&servaddr, sizeof(servaddr));
	
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1 forbidden function ?
	servaddr.sin_port = htons(port); 

	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
		exitError("Fatal Error");

	if (listen(sockfd, SOMAXCONN) != 0)
		exitError("Fatal Error");
	
	handle_socket(sockfd, servaddr);
	
	exit(0);
}