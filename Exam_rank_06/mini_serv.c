/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mini_serv.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/22 16:58:24 by adbenoit          #+#    #+#             */
/*   Updated: 2021/11/30 16:35:26 by adbenoit         ###   ########.fr       */
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
	int					fd;
	struct sockaddr_in	addr;
	struct s_client		*next;
}				t_client;

void    exitError(char *str, t_client *client)
{
	t_client	*it = client;
	
	while (client)
	{
		it = it->next;
		close(client->fd);
		free(client);
		client = it;
	}
	
	write(STDERR_FILENO, str, strlen(str));
	write(STDERR_FILENO, "\n", 1);
	exit(1);
}

char *str_join(char *buf, char *add)
{
	char	*newbuf;
	int		len;

	if (buf == 0)
		len = 0;
	else
		len = strlen(buf);
	newbuf = malloc(sizeof(*newbuf) * (len + strlen(add) + 1));
	if (newbuf == 0)
		return (0);
	newbuf[0] = 0;
	if (buf != 0)
		strcat(newbuf, buf);
	free(buf);
	strcat(newbuf, add);
	return (newbuf);
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

t_client	*ft_addnewclient(t_client *client, int fd)
{
	t_client	*it;
	
	if (!client)
	{
		client = (t_client *)malloc(sizeof(t_client));
		if (!client)
			exitError("Fatal Error", NULL);
		client->id = 0;	
		client->fd = fd;
		client->next = NULL;
		
		return (client);
	}
	
	/* get the last client */
	it = client;
	while (it->next)
		it = it->next;
		
	/* add the new client at the back of the list */
	it->next = (t_client *)malloc(sizeof(t_client));
	if (!it->next)
		exitError("Fatal Error", client);
	it->next->id = it->id + 1;	
	it->next->fd = fd;
	it->next->next = NULL;
	
	return (client);
}

t_client	*getClient(t_client *client, int fd)
{
	while (client && client->fd != fd)
		client = client->next;
	return (client);
}

int		ft_clientsize(t_client *client)
{
	int size = 0;

	while (client)
	{
		client = client->next;
		++size;
	}

	return (size);
}

int		get_maxfd(int sockfd, t_client *client)
{
	int maxfd = sockfd;
	t_client *tmp = client;

	while (tmp)
	{
		if (tmp->fd > maxfd)
			maxfd = tmp->fd;
		tmp = tmp->next;
	}

	return (maxfd);
}

t_client	*client_disconnect(t_client *client, t_client *disconn, struct fd_set *fds)
{
	t_client *it;
	
	it = client;
	while (it && it->next != disconn)
		it = it->next;
		
	FD_CLR(disconn->fd, fds);
	close(disconn->fd);
	printf(">client [\033[31m%d\033[0m] disconnected\n.", disconn->id);
	if (disconn->id != 0)
		it->next = disconn->next;
	else
		client = client->next;
	free(disconn);
	return (client);	
}

int	sendMessage(char *message, t_client *client, t_client *sender, struct fd_set *wfds)
{
	int	ret;
	
	while (client)
	{
		if (client != sender && FD_ISSET(client->fd, wfds))
		{
			ret = send(client->fd, message, strlen(message), 0);
			printf(">client %d: message sent\n", client->id);
			if (ret == -1)
				return (0);
		}
		client = client->next;
	}
	return (1);
}

t_client	*receiveMessage(int sockfd, t_client *client, t_client *sender,
			struct fd_set *rfds, struct fd_set *wfds, struct fd_set *allfds)
{
	char		message[4097];
	char		str[5000];
	int			ret;
	t_client	*it, *tmp;

	/* Server receive Message and resent it to all clients */
	if (FD_ISSET(sockfd, rfds))
	{
		ret = recv(sockfd, message, 4096, 0);
		if (ret != -1)
		{
			message[ret] = 0;
			write(STDIN_FILENO, message, strlen(message));
			sprintf(str, "client %d: %s\n", sender->id, message);
			sendMessage(str, client, sender, wfds);
			printf(">server: message sent\n");
		}
	}
	/* client receive message */
	else
	{
		it = client;
		while (it)
		{
			if (FD_ISSET(it->fd, rfds))
			{
				ret = recv(it->fd, message, 4096, 0);
				printf(">it %d: message received\n", it->id);
				message[ret] = 0;
				tmp = it;
				it = it->next;
				if (ret == 0)
					client = client_disconnect(client, tmp, allfds);
			}
		}
	}
	return (client);
}

void	handle_connection(int sockfd)
{
	int					connfd, retval, maxfd;
	struct sockaddr_in	addr_tmp;
	struct fd_set		rfds, wfds, allfds;
	socklen_t			len;
	t_client			*client;
	char				str[4096];

	FD_ZERO(&allfds);
	FD_SET(sockfd, &allfds);
	client = NULL;
	maxfd = sockfd;
	
	while (1)
	{
		rfds = allfds;
		wfds = allfds;

		/* waits for a connection */
		printf("\n>\033[31mWaiting for connection ...\033[0m\n");
		retval = select(maxfd + 1, &rfds, &wfds, NULL, NULL);
		/* error case */
		if (retval == -1)
			exitError("Fatal Error", NULL);
		/* connection founds */
		else if (retval > 0)
		{
			printf(">\033[33mConnection find.\033[0m\n");
			len = sizeof(struct sockaddr_in);
			connfd = accept(sockfd, (struct sockaddr *)&addr_tmp, &len);
			if (connfd < 0)
				exitError("Fatal Error", client);
			if (!FD_ISSET(connfd, &allfds))
			{
				client = ft_addnewclient(client, connfd);
				printf(">client [\033[32m%d\033[0m] connected\n", client->id);
				sprintf(str, "server: client %d just arrived\n", client->id);
				sendMessage(str, client, getClient(client, connfd), &wfds);
				FD_SET(connfd, &allfds);
				maxfd = get_maxfd(sockfd, client);
			}
			else
				printf(">client already connected\n");
			
			client = receiveMessage(sockfd, client, getClient(client, connfd), &rfds, &wfds, &allfds);
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

	/* Setup server */
	printf(">Server port : %d\n", port);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	bzero(&servaddr, sizeof(servaddr));
	
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(2130706433);
	servaddr.sin_port = htons(port); 

	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
		exitError("Fatal Error", NULL);

	if (listen(sockfd, SOMAXCONN) != 0)
		exitError("Fatal Error", NULL);
	
	handle_connection(sockfd);
	
	exit(0);
}