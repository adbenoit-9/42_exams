/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/24 15:21:38 by adbenoit          #+#    #+#             */
/*   Updated: 2021/06/24 19:23:46 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "microshell.h"

void    ft_cd(char **arg)
{
    if (!arg[1] || arg[2])
        write(2, "Error arg\n", 10);  
    else if (chdir(arg[1]) == -1)
        write(2, "Error chdir\n", 11);  
}

int    ft_execute(char **arg, char *envp[], int *fd_in, int *count)
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
    printf("next %d + 1: %s\n", i, arg[i + 1]);
    arg[i] = 0;
    pipe(fd);
    pid = fork();
    if (pid == 0)
    {
        dup2(*fd_in, 0);
        printf("last = %d\n", last);
        if (last == 0)
            dup2(fd[1], 1);
        // close(*fd_in);
	    // close(fd[0]);
	    // close(fd[1]);
        execve(arg[0], arg, envp);
        exit(0);
    }
    else if (pid == -1)
        write(2, "Error\n", 6);
    else
    {
        dup2(fd[0], *fd_in);
        // close(fd[0]);
		// close(fd[1]);
        if (!strcmp(arg[0], "cd"))
            ft_cd(arg);
        ++(*count);
    }
    return (i + 1);
}

void    ft_getCmd(char **arg, int end, char *envp[])
{
    int i;
    int status;
    int count;
    int fd_in;

    arg[end] = 0;
    fd_in = dup(0);
    printf("end = %d\n", end);
    i = 0;
    while (arg[i])
    {
        i += ft_execute(arg + i, envp, &fd_in, &count);
        printf("next %d: %s\n", i, arg[i]);
    }
    close(fd_in);
    while (count > 0)
    {
        waitpid(-1, &status, 0);
        --count;
    }
}
