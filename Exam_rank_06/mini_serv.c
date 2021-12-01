/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mini_serv.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/22 16:58:24 by adbenoit          #+#    #+#             */
/*   Updated: 2021/12/01 18:17:06 by adbenoit         ###   ########.fr       */
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

int get_message_end(char *msg)
{
	int	i;

	if (msg == 0)
		return (0);
	i = 0;
	while (msg[i])
	{
		if (msg[i] == '\n')
			return (i);
		i++;
	}
	return (0);
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

int	send_message(char *message, t_client *client, t_client *sender, struct fd_set *wfds)
{
	int	ret;
	
	while (client)
	{
		if (client != sender && FD_ISSET(client->fd, wfds))
		{
			ret = send(client->fd, message, strlen(message), 0);
			if (ret == -1)
				return (0);
		}
		client = client->next;
	}
	return (1);
}

t_client	*add_newclient(t_client *client, int fd, struct fd_set *allfds, struct fd_set *wfds)
{
	t_client	*it;
	char		str[4096];
	
	FD_SET(fd, allfds);
	if (!client)
	{
		client = (t_client *)malloc(sizeof(t_client));
		if (!client)
		{
			close(fd);
			exitError("Fatal Error", NULL);
		}
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

	sprintf(str, "server: client %d just arrived\n", it->next->id);
	send_message(str, client, it->next, wfds);

	return (client);
}

t_client	*delete_client(t_client *client, t_client *del, struct fd_set *fds,
				struct fd_set *wfds)
{
	char		str[4096];
	t_client	*it;
	
	it = client;
	while (it && it->next != del)
		it = it->next;
		
	sprintf(str, "server: client %d just left\n", del->id);
	send_message(str, client, del, wfds);
	
	FD_CLR(del->fd, fds);
	close(del->fd);
	if (client != del)
		it->next = del->next;
	else
		client = client->next;
	free(del);

	return (client);
}

/* check if the server receive a message and resent it to all clients */
t_client	*receive_message(t_client *client, struct fd_set *rfds,
				struct fd_set *wfds, struct fd_set *allfds)
{
	char		buffer[4097]; // a single message will never be longer than 4096 characters
	char		str[5000];
	int			ret;
	t_client	*it, *tmp;

	it = client;
	while (it)
	{
		/* message receives from it */
		if (FD_ISSET(it->fd, rfds))
		{
			ret = recv(it->fd, buffer, 4096, 0);
			tmp = it;
			it = it->next;
			/* client disconnects */
			if (ret == 0)
				client = delete_client(client, tmp, allfds, wfds);
			/* send messsage */
			else
			{
				buffer[ret] = 0;
				buffer[get_message_end(buffer) + 1] = 0;
				sprintf(str, "client %d: %s", it->id, buffer);
				send_message(str, client, it, wfds);
			}
		}
		else
			it = it->next;
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

	FD_ZERO(&allfds);
	FD_SET(sockfd, &allfds);
	client = NULL;
	maxfd = sockfd;
	
	while (1)
	{
		rfds = allfds;
		wfds = allfds;

		/* waits for a connection */
		retval = select(maxfd + 1, &rfds, &wfds, NULL, NULL);
		/* error case */
		if (retval == -1)
			exitError("Fatal Error", NULL);
		/* connection founds */
		else if (retval > 0)
		{
			len = sizeof(struct sockaddr_in);
			connfd = accept(sockfd, (struct sockaddr *)&addr_tmp, &len);
			if (connfd < 0)
				exitError("Fatal Error", client);
			if (!FD_ISSET(connfd, &allfds))
			{
				client = add_newclient(client, connfd, &allfds, &wfds);
				maxfd = get_maxfd(sockfd, client);
			}
			
			client = receive_message(client, &rfds, &wfds, &allfds);
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
