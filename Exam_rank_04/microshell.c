/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/29 15:09:39 by adbenoit          #+#    #+#             */
/*   Updated: 2021/12/03 17:39:44 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "microshell.h"

void    ft_cd(char **arg)
{
	if (!arg[1] || arg[2])                          // check the number of arguments
		ft_strerror("error: cd: bad arguments", NULL);
	else if (chdir(arg[1]) == -1)
		ft_strerror("error: cd: cannot change directory to ", arg[1]);
}

void    child_process(char **arg, int *fd, int fd_in, int last, char *envp[])
{
 		if (dup2(fd_in, STDIN_FILENO) == -1)        // reading the output of the previous command
 			ft_strerror("error: fatal", NULL); 
 		if (last == 0)
 			if (dup2(fd[1], STDOUT_FILENO) == -1)   // set the fd output
     			ft_strerror("error: fatal", NULL);
 		close(fd_in);
 		close(fd[0]);
 		close(fd[1]);
 		if (execve(arg[0], arg, envp) == -1)        // execute command
 			ft_strerror("error: cannot execute ", arg[0]);
}

int    ft_execute(char **arg, char *envp[], int fd_in)  
{
	pid_t pid;
	int fd[2];
	int i;
	int last;

	i = 0;
	last = 0;
	while (arg[i] && strcmp(arg[i], "|"))
		++i;
	if (!arg[i])                                // is this the last pipe ?
		last = 1;
	arg[i] = 0;                                 // to execute command until "|"
	if (pipe(fd) == -1 || (pid = fork()) == -1)
	    ft_strerror("error: fatal", NULL);
	else if (pid == 0)
       child_process(arg, fd, fd_in, last, envp);
	else
	{
		if (dup2(fd[0], fd_in) == -1)           // save fd[0] for the next command
   		    ft_strerror("error: fatal", NULL);
		close(fd[0]);
		close(fd[1]);
	}
	if (last == 0)
		return (i + 1);
	return (i);
}

void    launch_cmd(char **arg, char *envp[])
{
	int i;
	int status;
	int count;
	int fd_in;

	if (!arg[0])
		return ;
	count = 0;
    if (!strcmp(arg[0], "cd"))
			return (ft_cd(arg));
	if ((fd_in = dup(STDIN_FILENO)) == -1)      // duplicate the fd of the stdin
	    ft_strerror("error: fatal", NULL);
	i = 0;
	while (arg[i])
	{
		i += ft_execute(arg + i, envp, fd_in);
		++count;
	}
	close(fd_in);

    /*
    ** wait for all processes to terminated 
    */
	while (count > 0)
	{
		waitpid(-1, &status, 0);
		--count;
	}
}
