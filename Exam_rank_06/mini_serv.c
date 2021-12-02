/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mini_serv.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/02 13:50:09 by adbenoit          #+#    #+#             */
/*   Updated: 2021/12/02 13:59:10 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/select.h>

typedef struct s_client
{
	int					id;
	int					fd;
	struct sockaddr_in	addr;
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
		free(client);
		client = it;
	}
	write(STDERR_FILENO, str, strlen(str));
	write(STDERR_FILENO, "\n", 1);
	exit(1);
}

/* Get the position of the the last character of the message */
int	message_ending(char *msg)
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

int	get_maxfd(int sockfd, t_client *client)
{
	int			maxfd;
	t_client	*tmp;

	maxfd = sockfd;
	tmp = client;
	while (tmp)
	{
		if (tmp->fd > maxfd)
			maxfd = tmp->fd;
		tmp = tmp->next;
	}
	return (maxfd);
}

/* Send the message to all clients */
void	send_message(char *msg, t_client *client_lst, t_client *sender,
			t_fds *fds)
{
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
		exit_error("Fatal Error", client_lst);
	it->next->id = it->id + 1;
	it->next->fd = fd;
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
	char		buffer[4097]; // a single message will never be longer than 4096 characters
	char		str[5000];
	int			ret;
	t_client	*it, *tmp;

	it = client;
	while (it)
	{
		/* message receives from it */
		if (FD_ISSET(it->fd, &fds->read))
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
		else if (ret > 0)
		{
			len = sizeof(struct sockaddr_in);
			connfd = accept(sockfd, (struct sockaddr *)&addr_tmp, &len);
			if (!FD_ISSET(connfd, &fds.all))
			{
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
	exit(0);
}
