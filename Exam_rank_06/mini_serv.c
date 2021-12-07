/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mini_serv.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/02 13:50:09 by adbenoit          #+#    #+#             */
/*   Updated: 2021/12/07 16:33:26 by adbenoit         ###   ########.fr       */
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
	struct fd_set	all;	// all descriptors (server and clients)
	struct fd_set	rd;		// descriptors ready for reading
	struct fd_set	wr;		// descriptors ready for writing
	int				socket;
}				t_fds;


/**** begin of subject functions ****/
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
/**** end of subject funtions ****/


/* Clear client_lst and close fds */
void	exit_error(char *str, t_client *client_lst, int sockfd)
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
	if (sockfd != -1)
		close(sockfd);
	write(STDERR_FILENO, str, strlen(str));
	write(STDERR_FILENO, "\n", 1);
	exit(1);
}

/* Send the message to all clients */
void	send_message(char *msg, t_client *client_lst, t_client *sender,
			t_fds *fds)
{
	write(STDIN_FILENO, msg, strlen(msg));
	while (client_lst)
	{
		if (client_lst != sender && FD_ISSET(client_lst->fd, &fds->wr))
			send(client_lst->fd, msg, strlen(msg), 0);
		client_lst = client_lst->next;
	}
}

/* Adds a client to the list and informs the others of his arrival. */
t_client	*add_newclient(t_client *client_lst, int fd, int id, t_fds *fds)
{
	t_client	*newcli, *last;
	char		str[4096];

	
	newcli = (t_client *)malloc(sizeof(t_client));
	if (!newcli)
	{
		close(fd);
		exit_error("Fatal Error", client_lst, fds->socket);
	}
	newcli->id = id;
	newcli->fd = fd;
	newcli->buffer = NULL;
	newcli->next = NULL;
	FD_SET(fd, &fds->all);
	sprintf(str, "server: client %d just arrived\n", id);
	send_message(str, client_lst, newcli, fds);
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
t_client	*remove_client(t_client *client_lst, t_client *rmcli, t_fds *fds)
{
	char		str[4096];
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
	send_message(str, client_lst, rmcli, fds);
	FD_CLR(rmcli->fd, &fds->all);
	close(rmcli->fd);
	free(rmcli->buffer);
	free(rmcli);
	return (client_lst);
}

/* Check if the server receive a message and resend it to all clients. */
t_client	*receive_message(t_client *client_lst, t_fds *fds)
{
	char		buffer[BUFFER_SIZE];
	char		*msg;
	char		str[5000];
	int			ret;
	t_client	*it, *curr_cli;

	it = client_lst;
	while (it)
	{
		curr_cli = it;
		it = curr_cli->next;
		/* message receives from curr_cli */
		if (FD_ISSET(curr_cli->fd, &fds->rd))
		{
			ret = recv(curr_cli->fd, buffer, BUFFER_SIZE - 1, 0);
			/* client disconnects */
			if (ret == 0)
				client_lst = remove_client(client_lst, curr_cli, fds);
			/* send messsage */
			else if (ret != -1)
			{
				buffer[ret] = 0;
				curr_cli->buffer = str_join(curr_cli->buffer, buffer);
				if (!curr_cli->buffer)
					exit_error("Fatal Error", client_lst, fds->socket);
				while ((ret = extract_message(&curr_cli->buffer, &msg)))
				{
					sprintf(str, "client %d: %s", curr_cli->id, msg);
					send_message(str, client_lst, curr_cli, fds);
					free(msg);
				}
				if (ret == -1)
					exit_error("Fatal Error", client_lst, fds->socket);
					
			}
		}
	}
	return (client_lst);
}

/* Waits for a connection on the server and processes it. */
void	handle_connection(int sockfd)
{
	int					connfd, ret, maxfd, client_id;
	struct sockaddr_in	addr;
	t_fds				fds;
	socklen_t			len;
	t_client			*client_lst;

	fds.socket = sockfd;
	FD_ZERO(&fds.all);
	FD_SET(sockfd, &fds.all);
	client_lst = NULL;
	maxfd = sockfd;
	client_id = 0;
	while (1)
	{
		FD_COPY(&fds.all, &fds.rd);
		FD_COPY(&fds.all, &fds.wr);
		/* waits for a connection */
		ret = select(maxfd + 1, &fds.rd, &fds.wr, NULL, NULL);
		/* error case */
		if (ret == -1)
			exit_error("Fatal Error", NULL, fds.socket);
		/* connection founds */
		else if (ret > 0 && FD_ISSET(sockfd, &fds.rd))
		{
			len = sizeof(struct sockaddr_in);
			connfd = accept(sockfd, (struct sockaddr *)&addr, &len);
			fcntl(connfd, F_SETFL, O_NONBLOCK);
			if (connfd > 0)
			{
				client_lst = add_newclient(client_lst, connfd, client_id, &fds);
				maxfd = connfd > maxfd ? connfd : maxfd;
				++client_id;
			}
		}
		else if (client_lst && ret > 0)
			client_lst = receive_message(client_lst, &fds);
	}
}

int	main(int ac, char **av)
{
	int					port, sockfd;
	struct sockaddr_in	servaddr;

	if (ac < 2)
		exit_error("Wrong number of arguments", NULL, -1);
	port = atoi(av[1]);
	/* Setup server */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
		exit_error("Fatal Error", NULL, -1);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(2130706433); // 127.0.0.1
	servaddr.sin_port = htons(port);
	if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)))
		exit_error("Fatal Error", NULL, sockfd);
	if (listen(sockfd, SOMAXCONN) != 0)
		exit_error("Fatal Error", NULL, sockfd);
	handle_connection(sockfd);
	return (0);
}
