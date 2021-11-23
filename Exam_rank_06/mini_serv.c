/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mini_serv.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/22 16:58:24 by adbenoit          #+#    #+#             */
/*   Updated: 2021/11/23 13:53:45 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>

void    exitError(char *str)
{
	write(STDERR_FILENO, str, strlen(str));
	write(STDERR_FILENO, "\n", 1);
	exit(1);
}

int main (int ac, char **av)
{
	int 		port;
	int 		fd;
	sockaddr_in sock;

	if (ac < 2)
		exitError("Wrong number of arguments");
	port = atoi(av[1]);
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
		exitError("Fatal Error");
}