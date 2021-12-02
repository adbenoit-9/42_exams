/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mini_serv.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/02 13:50:09 by adbenoit          #+#    #+#             */
/*   Updated: 2021/12/02 19:03:37 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/select.h>
#include <fcntl.h>

#define BUFFER_SIZE 65535

typedef struct s_client
{
	int					id;
	int					fd;
	char				*buffer;
	struct s_client		*next;
}				t_client;

typedef struct s_fds
{
	struct fd_set	all;
	struct fd_set	read;
	struct fd_set	write;
}				t_fds;

void	exit_error(char *str, t_client *client)
{
	t_client	*it;

	it = client;
	while (client)
	{
		it = it->next;
		close(client->fd);
		free(client->buffer);
		free(client);
		client = it;
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

int	get_maxfd(int sockfd, t_client *client)
{
	int			maxfd;

	maxfd = sockfd;
	while (client)
	{
		if (client->fd > maxfd)
			maxfd = client->fd;
		client = client->next;
	}
	return (maxfd);
}

/* Send the message to all clients */
void	send_message(char *msg, t_client *client_lst, t_client *sender,
			t_fds *fds)
{
	write(STDIN_FILENO, msg, strlen(msg));
	while (client_lst)
	{
		if (client_lst != sender && FD_ISSET(client_lst->fd, &fds->write))
			send(client_lst->fd, msg, strlen(msg), 0);
		client_lst = client_lst->next;
	}
}

/* Adds a client to the list and informs the others of his arrival. */
t_client	*add_newclient(t_client *client_lst, int fd, t_fds *fds)
{
	t_client	*it;
	char		str[4096];

	FD_SET(fd, &fds->all);
	if (!client_lst)
	{
		client_lst = (t_client *)malloc(sizeof(t_client));
		if (!client_lst)
		{
			close(fd);
			exit_error("Fatal Error", NULL);
		}
		client_lst->id = 0;
		client_lst->fd = fd;
		client_lst->buffer = NULL;
		client_lst->next = NULL;
		send_message("server: client 0 just arrived\n", client_lst, client_lst, fds);
		return (client_lst);
	}
	/* get the last client */
	it = client_lst;
	while (it->next)
		it = it->next;
	/* adds the new client at the back of the list */
	it->next = (t_client *)malloc(sizeof(t_client));
	if (!it->next)
		exit_error("Fatal Error", client_lst);
	it->next->id = it->id + 1;
	it->next->fd = fd;
	it->next->buffer = NULL;
	it->next->next = NULL;
	sprintf(str, "server: client %d just arrived\n", it->next->id);
	send_message(str, client_lst, it->next, fds);
	return (client_lst);
}

/* Removes a client from the list, disconnect him and informs the others. */
t_client	*remove_client(t_client *client_lst, t_client *del, t_fds *fds)
{
	char		str[4096];
	t_client	*it;

	it = client_lst;
	while (it && it->next != del)
		it = it->next;
	sprintf(str, "server: client %d just left\n", del->id);
	send_message(str, client_lst, del, fds);
	FD_CLR(del->fd, &fds->all);
	close(del->fd);
	if (client_lst != del)
		it->next = del->next;
	else
		client_lst = client_lst->next;
	free(del);
	return (client_lst);
}

/* Check if the server receive a message and resend it to all clients. */
t_client	*receive_message(t_client *client, t_fds *fds)
{
	char		buffer[BUFFER_SIZE];
	char		*msg;
	char		str[5000];
	int			ret;
	t_client	*it, *tmp;

	it = client;
	while (it)
	{
		/* message receives from it */
		if (FD_ISSET(it->fd, &fds->read))
		{
			ret = recv(it->fd, buffer, BUFFER_SIZE - 1, MSG_WAITALL);
			tmp = it;
			it = it->next;
			/* client disconnects */
			if (ret == 0)
				client = remove_client(client, tmp, fds);
			/* send messsage */
			else if (ret != -1)
			{
				buffer[ret] = 0;
				it->buffer = str_join(it->buffer, buffer);
				while (extract_message(&it->buffer, &msg))
				{
					sprintf(str, "client %d: %s", it->id, msg);
					send_message(str, client, it, fds);
					free(msg);
				}
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
	t_fds				fds;
	socklen_t			len;
	t_client			*client;

	FD_ZERO(&fds.all);
	FD_SET(sockfd, &fds.all);
	client = NULL;
	maxfd = sockfd;
	while (1)
	{
		fds.read = fds.all;
		fds.write = fds.all;
		/* waits for a connection */
		ret = select(maxfd + 1, &fds.read, &fds.write, NULL, NULL);
		/* error case */
		if (ret == -1)
			exit_error("Fatal Error", NULL);
		/* connection founds */
		else if (ret > 0 && FD_ISSET(sockfd, &fds.read))
		{
			len = sizeof(struct sockaddr_in);
			connfd = accept(sockfd, (struct sockaddr *)&addr_tmp, &len);
			if (!FD_ISSET(connfd, &fds.all))
			{
				// fcntl(connfd, F_SETFL, O_NONBLOCK);
				client = add_newclient(client, connfd, &fds);
				maxfd = get_maxfd(sockfd, client);
			}
			client = receive_message(client, &fds);
		}
	}
}

int	main(int ac, char **av)
{
	int					port, sockfd;
	struct sockaddr_in	servaddr;

	if (ac < 2)
		exit_error("Wrong number of arguments", NULL);
	port = atoi(av[1]);
	/* Setup server */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
		exit_error("Fatal Error", NULL);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(2130706433);
	servaddr.sin_port = htons(port);
	if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)))
		exit_error("Fatal Error", NULL);
	if (listen(sockfd, SOMAXCONN) != 0)
		exit_error("Fatal Error", NULL);
	handle_connection(sockfd);
	close(sockfd);
	exit(0);
}
