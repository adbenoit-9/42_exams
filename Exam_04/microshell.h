/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/24 15:14:49 by adbenoit          #+#    #+#             */
/*   Updated: 2021/06/24 19:09:15 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MICROSHELL_H
# define MICROSHELL_H

# include <unistd.h>
# include <sys/wait.h>
# include <string.h>
# include <stdlib.h>
# include <stdio.h>


void    ft_getCmd(char **arg, int end, char *envp[]);

#endif
