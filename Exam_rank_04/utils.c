/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/29 14:31:51 by adbenoit          #+#    #+#             */
/*   Updated: 2021/12/03 17:49:53 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "microshell.h"

size_t	ft_strlen(char *str)
{
	int i;

 	i = 0;
    while (str[i])
		++i;
	return (i);

}

void	ft_strerror(char *str, char *arg)
{
	write(STDERR_FILENO, str, ft_strlen(str));
	if (arg)
		write(STDERR_FILENO, arg, ft_strlen(arg));
	write(STDERR_FILENO, "\n", 1);
	kill(0, SIGINT);
}
 