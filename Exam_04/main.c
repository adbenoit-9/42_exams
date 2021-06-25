/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/24 15:16:13 by adbenoit          #+#    #+#             */
/*   Updated: 2021/06/25 14:14:16 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "microshell.h"

int main(int ac, char **av, char *envp[])
{
    int i;
    int start;

    i = 1;
    printf("cmd : ");
    while (av[i])
    {
        printf("%s ", av[i]);
        ++i;
    }
    printf("\n");
    i = 1;
    start = 1;
    while (av[i])
    {
        if (strcmp(av[i], ";") == 0)
        {
            ft_getCmd(av + start, i - start, envp);
            start = i + 1;
        }
        ++i;
    }
    ft_getCmd(av + start, i, envp);
}
