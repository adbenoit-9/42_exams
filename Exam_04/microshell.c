/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/24 15:21:38 by adbenoit          #+#    #+#             */
/*   Updated: 2021/06/25 17:39:40 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "microshell.h"
#include <sys/errno.h>

size_t	ft_strlen(char *str)
{
	int i;

	i = 0;
	while (str[i])
		++i;
	return (i);

}

void	message_error(char *str, char *arg)
{
	write(2, str, ft_strlen(str));
	if (arg)
		write(2, arg, ft_strlen(arg));
	write(2, "\n", 1);
}

void	fatal_error(void)
{
	write(2, "error: fatal\n", 14);
	kill(0, SIGKILL);
}

void    ft_cd(char **arg)
{
	if (!arg[1] || arg[2])
		message_error("error: cd: bad arguments", NULL);
	else if (chdir(arg[1]) == -1)
		message_error("error: cd: cannot change directory to ", arg[1]);
	exit(0);
}

int    ft_execute(char **arg, char *envp[], int *fd_in)
{
	pid_t pid;
	int fd[2];
	int i;
	int last;

	i = 0;
	last = 1;
	while (arg[i] && strcmp(arg[i], "|"))
		++i;
	if (arg[i])
		last = 0;
	arg[i] = 0;
	if (pipe(fd) == -1 || (pid = fork()) == -1)
		fatal_error();
	if (pid == 0)
	{
		if (dup2(*fd_in, 0) == -1)
			fatal_error();
		if (last == 0)
			if (dup2(fd[1], 1) == -1)
				fatal_error();
		close(*fd_in);
		close(fd[0]);
		close(fd[1]);
		if (strcmp(arg[0], "cd") && execve(arg[0], arg, envp) == -1)
			message_error("error: cannot execute ", arg[0]);
	}
	else
	{
		if (dup2(fd[0], *fd_in) == -1)
			fatal_error();
		close(fd[0]);
		close(fd[1]);
		if (!strcmp(arg[0], "cd"))
			ft_cd(arg);
	}
	if (last == 0)
		return (i + 1);
	return (i);
}

void    ft_getCmd(char **arg, int end, char *envp[])
{
	int i;
	int status;
	int count;
	int fd_in;

	arg[end] = 0;
	if ((fd_in = dup(0)) == -1)
		fatal_error();
	i = 0;
	while (arg[i])
	{
		i += ft_execute(arg + i, envp, &fd_in);
		++count;
	}
	close(fd_in);
	while (count > 0)
	{
		waitpid(-1, &status, 0);
		--count;
	}
}
