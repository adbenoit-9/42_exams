/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mini_paint.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/13 02:30:14 by adbenoit          #+#    #+#             */
/*   Updated: 2021/01/13 02:33:00 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINI_PAINT_H
# define MINI_PAINT_H

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
	float	rad;
	float	x;
	float	y;
}				t_draw;

int		    ft_strlen(char *str);
int		    ft_putstr(char *str);
void	    ft_puttab(char **tab);
int		    ft_freetab(char **tab);

#endif