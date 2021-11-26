/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mini_serv.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/22 16:58:24 by adbenoit          #+#    #+#             */
/*   Updated: 2021/11/26 15:18:39 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/select.h>

typedef struct	s_client
{
	int					id;
	int					connfd;
	struct sockaddr_in	addr;
	struct s_client		next;
}				t_client;

void    exitError(char *str, t_client *client_list)
{
	t_client	tmp = client_list;
	
	while (client_list)
	{
		tmp = tmp->next;
		free(client_list);
		client_list = tmp;
	}
	
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

t_client	*ft_addnewclient(t_client *client_list, struct sockaddr_in addr, int fd)
{
	t_client	tmp;
	
	if (!client_list)
	{
		client_list = (t_client *)malloc(sizeof(t_client *));
		if (!client_list)
			exitError("Fatal Error", NULL);
		client_list->id = 0;	
		client_list->addr = addr;
		client_list->connfd = fd;
		client_list->next = NULL;
		
		return client_list;
	}
	
	tmp = client_list;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = (t_client *)malloc(sizeof(t_client *));
	if (!tmp->next)
			exitError("Fatal Error", client_list);
	tmp->next->id = tmp->id + 1;	
	tmp->next->addr = addr;
	tmp->next->connfd = fd;
	tmp->next->next = NULL;
	
	return client_list;
}

void	handle_socket(int sockfd, struct sockaddr_in servaddr)
{
	int					connfd, retval, client_id;
	struct sockaddr_in	addr_tmp;
	fd_set				rfd, wfd;
	socklen_t			len;
	t_client			*client_list;
	char				str[80];

	clients = NULL;
	while (true)
	{
		/* waits for a connection */
		retval = select(2, &rfd, &wfd, NULL, NULL);
		
		/* error case */
		if (retval == -1)
			exitError("Fatal Error", NULL);
		/* connexion founds */
		else if (retval > 0)
		{
			len = sizeof(struct sockaddr_in);
			connfd = accept(sockfd, (struct sockaddr *)&addr_tmp, &len);
			if (connfd < 0)
				exitError("Fatal Error", client_list);

			client_list = ft_addnewclient(client_list, addr_tmp, connfd);
			
			/*
			** 1 - check if the client is already connected. not util ?
			** 2 - check if he is trying to read or send a message.
			** 3 - process the action (request or response).
			** 4 - if the client try to send a message, send it to all clients connected.

			** How to reconize the action of a client with select ?
			** How to know when a client diconnects ? at the end of the request or when i close the fd ?
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
		exitError("Wrong number of arguments", NULL);
	port = atoi(av[1]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	bzero(&servaddr, sizeof(servaddr));
	
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1 forbidden function ? => AF_INET
	servaddr.sin_port = htons(port); 

	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
		exitError("Fatal Error", NULL);

	if (listen(sockfd, SOMAXCONN) != 0)
		exitError("Fatal Error", NULL);
	
	handle_socket(sockfd, servaddr);
	
	exit(0);
}