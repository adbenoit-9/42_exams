/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/29 13:49:50 by adbenoit          #+#    #+#             */
/*   Updated: 2021/06/29 15:19:25 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "microshell.h"

int main(int ac, char **av, char *envp[])
{
    int i;
    int start;

    if (ac < 2)
        return (0);
    i = 1;
    start = 1; // skip a.out
    while (av[i])
    {
        if (strcmp(av[i], ";") == 0)
        {
            av[i] = 0; // to use the av until ";" => one command
            lauch_cmd(av + start, envp);
            ++i;
            while (av[i] && strcmp(av[i], ";") == 0)
               ++i;
            start = i; // next command is after ";"
        }
        else
           ++i;
    }
    if (av[start])
        lauch_cmd(av + start, envp);
    return (0);
}
 