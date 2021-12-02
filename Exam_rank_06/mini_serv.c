/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mini_serv.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/22 16:58:24 by adbenoit          #+#    #+#             */
/*   Updated: 2021/12/02 13:02:13 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/select.h>

#define ALL 0
#define READ 1
#define WRITE 2

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

/* Get the position of the the last character of the message */
int message_ending(char *msg)
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

/* Send the message to all clients */
void	send_message(char *msg, t_client *client_lst, t_client *sender, struct fd_set **fds)
{
	while (client_lst)
	{
		if (client_lst != sender && FD_ISSET(client_lst->fd, fds[WRITE]))
			send(client_lst->fd, msg, strlen(msg), 0);
		client_lst = client_lst->next;
	}
}

/* Adds a client to the list and informs the others of his arrival. */
t_client	*add_newclient(t_client *client_lst, int fd, struct fd_set **fds)
{
	t_client	*it;
	char		str[4096];
	
	FD_SET(fd, fds[ALL]);
	if (!client_lst)
	{
		client_lst = (t_client *)malloc(sizeof(t_client));
		if (!client_lst)
		{
			close(fd);
			exitError("Fatal Error", NULL);
		}
		client_lst->id = 0;	
		client_lst->fd = fd;
		client_lst->next = NULL;
		return (client_lst);
	}

	/* get the last client */
	it = client_lst;
	while (it->next)
		it = it->next;

	/* adds the new client at the back of the list */
	it->next = (t_client *)malloc(sizeof(t_client));
	if (!it->next)
		exitError("Fatal Error", client_lst);
	it->next->id = it->id + 1;	
	it->next->fd = fd;
	it->next->next = NULL;

	sprintf(str, "server: client %d just arrived\n", it->next->id);
	send_message(str, client_lst, it->next, fds);

	return (client_lst);
}

/* Removes a client from the list, disconnect him and informs the others. */
t_client	*remove_client(t_client *client_lst, t_client *del, struct fd_set **fds)
{
	char		str[4096];
	t_client	*it;
	
	it = client_lst;
	while (it && it->next != del)
		it = it->next;
		
	sprintf(str, "server: client %d just left\n", del->id);
	send_message(str, client_lst, del, fds);
	
	FD_CLR(del->fd, fds[ALL]);
	close(del->fd);
	if (client_lst != del)
		it->next = del->next;
	else
		client_lst = client_lst->next;
	free(del);

	return (client_lst);
}

/* Check if the server receive a message and resend it to all clients. */
t_client	*receive_message(t_client *client, struct fd_set **fds)
{
	char		buffer[4097]; // a single message will never be longer than 4096 characters
	char		str[5000];
	int			ret;
	t_client	*it, *tmp;

	it = client;
	while (it)
	{
		/* message receives from it */
		if (FD_ISSET(it->fd, fds[READ]))
		{
			ret = recv(it->fd, buffer, 4096, 0);
			tmp = it;
			it = it->next;
			/* client disconnects */
			if (ret == 0)
				client = remove_client(client, tmp, fds);
			/* send messsage */
			else
			{
				buffer[ret] = 0;
				buffer[message_ending(buffer) + 1] = 0;
				sprintf(str, "client %d: %s", it->id, buffer);
				send_message(str, client, it, fds);
			}
		}
		else
			it = it->next;
	}
	
	return (client);
}

/* Waits for a connection on the server and processes it. */
void	handle_connection(int sockfd)
{
	int					connfd, ret, maxfd;
	struct sockaddr_in	addr_tmp;
	struct fd_set		*fds[3];
	socklen_t			len;
	t_client			*client;

	FD_ZERO(fds[ALL]);
	FD_SET(sockfd, fds[ALL]);
	client = NULL;
	maxfd = sockfd;
	
	while (1)
	{
		*fds[READ] = *fds[ALL];
		*fds[WRITE] = *fds[ALL];

		/* waits for a connection */
		ret = select(maxfd + 1, fds[READ], fds[WRITE], NULL, NULL);
		/* error case */
		if (ret == -1)
			exitError("Fatal Error", NULL);
		/* connection founds */
		else if (ret > 0)
		{
			len = sizeof(struct sockaddr_in);
			connfd = accept(sockfd, (struct sockaddr *)&addr_tmp, &len);
			if (!FD_ISSET(connfd, fds[ALL]))
			{
				client = add_newclient(client, connfd, fds);
				maxfd = get_maxfd(sockfd, client);
			}
			
			client = receive_message(client, fds);
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
		exitError("Fatal Error", NULL);
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
