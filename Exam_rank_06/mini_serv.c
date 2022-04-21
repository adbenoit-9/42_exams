/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mini_serv.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/02 13:50:09 by adbenoit          #+#    #+#             */
/*   Updated: 2022/04/14 15:20:52 by adbenoit         ###   ########.fr       */
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

typedef struct s_client
{
	int					id;
	int					fd;
	char				*buffer;
	struct s_client		*next;
}				t_client;

fd_set	fd_all;		// all descriptors (server and clients)
fd_set	fd_rd;		// descriptors ready for reading
fd_set	fd_wr;		// descriptors ready for writing

/**** begin of subject functions ****/
int	extract_message(char **buf, char **msg)
{
	char	*newbuf;
	int		i;

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

char	*str_join(char *buf, char *add)
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
/**** end of subject funtions ****/

/* Clear client_lst and close fds */
void	exit_error(char *str, t_client *client_lst)
{
	t_client	*tmp;

	tmp = client_lst;
	while (client_lst)
	{
		tmp = tmp->next;
		close(client_lst->fd);
		free(client_lst->buffer);
		free(client_lst);
		client_lst = tmp;
	}
	write(STDERR_FILENO, str, strlen(str));
	exit(1);
}

/* Send the message to all clients */
void	send_message(char *msg, t_client *client_lst, t_client *sender)
{
	while (client_lst)
	{
		if (client_lst != sender && FD_ISSET(client_lst->fd, &fd_wr))
			send(client_lst->fd, msg, strlen(msg), 0);
		client_lst = client_lst->next;
	}
}

/* Adds a client to the list and informs the others of his arrival. */
t_client	*add_newclient(t_client *client_lst, int fd, int id)
{
	t_client	*newcli, *last;
	char		str[1000];

	newcli = (t_client *)malloc(sizeof(t_client));
	if (!newcli)
		exit_error("Fatal Error\n", client_lst);
	newcli->id = id;
	newcli->fd = fd;
	newcli->buffer = NULL;
	newcli->next = NULL;
	FD_SET(fd, &fd_all);
	sprintf(str, "server: client %d just arrived\n", id);
	send_message(str, client_lst, newcli);
	if (!client_lst)
		return (newcli);
	/* adds the new client at the back of the list */
	last = client_lst;
	while (last->next)
		last = last->next;
	last->next = newcli;
	return (client_lst);
}

/* Removes a client from the list, disconnect him and informs the others. */
t_client	*remove_client(t_client *client_lst, t_client *rmcli)
{
	char		str[1000];
	t_client	*prev;

	if (client_lst == rmcli)
		client_lst = rmcli->next;
	else
	{
		prev = client_lst;
		while (prev->next != rmcli)
			prev = prev->next;
		prev->next = rmcli->next;
	}
	sprintf(str, "server: client %d just left\n", rmcli->id);
	send_message(str, client_lst, rmcli);
	FD_CLR(rmcli->fd, &fd_all);
	close(rmcli->fd);
	free(rmcli->buffer);
	free(rmcli);
	return (client_lst);
}

/* Receives the entire message. Returns his size. */
ssize_t	receive_message(t_client *client_lst, t_client *curr_cli)
{
	ssize_t		size, ret;
	char		buffer[1001];

	size = 0;
	while ((ret = recv(curr_cli->fd, buffer, 1000, 0)) > 0)
	{
		size += ret;
		buffer[ret] = 0;
		curr_cli->buffer = str_join(curr_cli->buffer, buffer);
		if (!curr_cli->buffer)
			exit_error("Fatal Error\n", client_lst);
	}
	if (size == 0 && ret == -1)
		return (-1);
	return (size);
}

/* Check if a client sent a message and resend it to all other clients. */
t_client	*handle_clients(t_client *client_lst)
{
	char		*msg, *str;
	ssize_t		ret;
	t_client	*it, *curr_cli;

	it = client_lst;
	while (it)
	{
		curr_cli = it;
		it = curr_cli->next;
		/* message receives from curr_cli */
		if (FD_ISSET(curr_cli->fd, &fd_rd))
		{
			ret = receive_message(client_lst, curr_cli);
			/* client disconnects */
			if (ret == 0)
				client_lst = remove_client(client_lst, curr_cli);
			/* resend messsage line by line */
			else if (ret > 0)
			{
				while ((ret = extract_message(&curr_cli->buffer, &msg)) == 1)
				{
					if (!(str = malloc(sizeof(char) * (20 + strlen(msg)))))
						exit_error("Fatal Error\n", client_lst);
					sprintf(str, "client %d: %s", curr_cli->id, msg);
					send_message(str, client_lst, curr_cli);
					free(msg);
					free(str);
				}
				if (ret == -1)
					exit_error("Fatal Error\n", client_lst);
			}
		}
	}
	return (client_lst);
}

/* handle connections on the server and processes it. */
void	handle_server(int sockfd)
{
	int					connfd, maxfd, client_id;
	t_client			*client_lst;

	FD_ZERO(&fd_all);
	FD_SET(sockfd, &fd_all);
	client_lst = NULL;
	maxfd = sockfd;
	client_id = 0;
	while (1)
	{
		fd_rd = fd_all;
		fd_wr = fd_all;
		/* select ready fds */
		if (select(maxfd + 1, &fd_rd, &fd_wr, NULL, NULL) < 0)
			continue ;
		/* new connection */
		else if (FD_ISSET(sockfd, &fd_rd))
		{
			connfd = accept(sockfd, NULL, NULL);
			if (connfd >= 0)
			{
				// fcntl(connfd, F_SETFL, O_NONBLOCK);
				client_lst = add_newclient(client_lst, connfd, client_id);
				maxfd = connfd > maxfd ? connfd : maxfd;
				++client_id;
			}
		}
		else
			client_lst = handle_clients(client_lst);
	}
}

int	main(int ac, char **av)
{
	int					port, sockfd;
	struct sockaddr_in	servaddr;

	if (ac < 2)
		exit_error("Wrong number of arguments\n", NULL);
	port = atoi(av[1]);
	/* Setup server */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
		exit_error("Fatal Error\n", NULL);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(2130706433); // 127.0.0.1
	servaddr.sin_port = htons(port);
	if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)))
		exit_error("Fatal Error\n", NULL);
	if (listen(sockfd, 10) != 0)
		exit_error("Fatal Error\n", NULL);
	handle_server(sockfd);
	return (0);
}
