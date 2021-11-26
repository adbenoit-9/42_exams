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

int		ft_clientsize(t_client *client_list)
{
	t_client *tmp = client_list;
	int size = 0;

	while (tmp)
	{
		++size;
		tmp = tmp->next;
	}

	return size;
}

int		get_maxfd(int socketfd, t_client *client_list)
{
	int maxfd = socketfd;
	t_client tmp = client_list;

	while(tmp)
	{
		if (tmp->connfd > maxfd)
			maxfd = tmp->connfd;
		tmp = tmp->next;
	}

	return maxfd;
}

void	receiveMessage(int socketfd, t_client *client_list, int client_id, fd_set rfds)
{
	char message[4097];
	char str[4096];
	char *newMessage;
	int ret;

	if (!FD_ISSET(socketfd, rfds))
		return ;
	ret = recv(socketfd, message, 4096, 0);
	message[ret] = 0;
	sprintf(str, "client %d: ", client_id);
	newMessage = str_join(str, message);
	sendMessage(newMessage, client_list);
	free(newMessage);
}

void	sendMessage(char *message, t_client *client_list, set_fd wfds)
{
	t_client *tmp = client_list;
	while (tmp)
	{
		// vraiemnt pas sure
		if (FD_ISSET(tmp->connfd, wfds))
			// send ?
			// write(STDIN_FILENO, message, strlen(message));
		else
			// client disconnect ? delete it end send to
		tmp = tmp->next;
	}
}

void	handle_socket(int sockfd, struct sockaddr_in servaddr)
{
	int					connfd, retval, client_id, maxfd;
	struct sockaddr_in	addr_tmp;
	fd_set				rfd, wfd, allfd;
	socklen_t			len;
	t_client			*client_list;
	char				str[4096];

	FD_SET(socketfd, &allfd);
	clients = NULL;
	maxfd = socketfd;
	while (true)
	{
		rfd = allfd;
		wfd = allfd;

		/* waits for a connection */
		retval = select(maxfd, &rfd, &wfd, NULL, NULL);
		
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
			if (!FD_ISSET(connfd, allfd))
			{
				client_list = ft_addnewclient(client_list, addr_tmp, connfd);
				sprintf(str, "server: client %d just arrived\n", client_id);
				sendMessage(str, client_list, wfd);
				FD_SET(connfd, &allfd);
				maxfd = get_maxfd(socketfd, client_list);
			}

			receiveMessage(socketfd, client_list, ft_clientsize() - 1, rfd);
			/*
			** 1 - check if the client is already connected => fd_isset allfd
			** 2 - check if he is trying to read or send a message => fd_isset rfd/wfd
			** 3 - process the action (request or response).
			** 4 - if the client try to send a message, send it to all clients connected.

			** How to know when a client diconnects ? at the end of the request or when i close the fd ?
			*/
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