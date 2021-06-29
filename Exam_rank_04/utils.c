/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/29 14:31:51 by adbenoit          #+#    #+#             */
/*   Updated: 2021/06/29 16:02:08 by adbenoit         ###   ########.fr       */
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
	// write(2, "\n", 1); kill print \n sur stderr ?
	kill(0, SIGINT);
}
 