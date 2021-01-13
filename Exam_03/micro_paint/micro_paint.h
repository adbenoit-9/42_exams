/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   micro_paint.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/13 13:42:50 by adbenoit          #+#    #+#             */
/*   Updated: 2021/01/13 14:12:37 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MICRO_PAINT_H
# define MICRO_PAINT_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <math.h>
# include <string.h>

typedef struct s_zone
{
	int		w;
	int		h;
	char	bc;
}				t_zone;

typedef struct s_draw
{	
	char	type;
	char	c;
	float	x;
	float	y;
	float	h;
	float	w;
}				t_draw;

int		    ft_strlen(char *str);
int		    ft_putstr(char *str);
void	    ft_puttab(char **tab);
int		    ft_freetab(char **tab);

#endif