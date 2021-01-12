/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   my_mini_paint.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/12 22:46:32 by adbenoit          #+#    #+#             */
/*   Updated: 2021/01/12 22:52:24 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

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

/*
** utils functions
*/

int		ft_strlen(char *str)
{
	int	i;

	i = 0;
	while (str[i])
		++i;
	return (i);
}

int		ft_putstr(char *str)
{
	write(1, str, ft_strlen(str));
	return (1);
}

void		ft_puttab(char **tab)
{
	int	i;

	i = 0;
	while (tab[i])
	{
		ft_putstr(tab[i]);
		write(1, "\n", 1);
		++i;
	}
}

int		ft_freetab(char **tab)
{
	int	i;

	i = 0;
	while (tab[i])
	{
		free(tab[i]);
		++i;
	}
	free(tab);
	return (1);
}

/*
** mini_paint functions
*/

int		is_in_circle(float xa, float ya, t_draw draw)
{
	float	d;

	d = sqrtf((xa - draw.x) * (xa - draw.x) + (ya - draw.y) * (ya - draw.y));
	if (d <= draw.rad)
	{
		if ((draw.rad - d) < 1.0)
			return (1);
		return (0);
	}
	return (-1);
	
}

int		draw_circle(t_draw draw, t_zone zone, char **paint)
{
	int	i;
	int	j;
	int	k;

	i = 0;
	while (i < zone.h)
	{
		j = 0;
		while (j < zone.w)
		{
			k = is_in_circle(j, i, draw);
			if (k == 1 || (draw.type == 'C' && k == 0))
				paint[i][j] = draw.c;
			++j;
		}
		++i;
	}
	return (0);
}

void		init_draw(t_draw *draw)
{
	draw->type = 0;
	draw->c = 0;
	draw->rad = 0;
	draw->x = 0;
	draw->y = 0;
}

int			init_paint(char **paint, t_zone zone)
{
	int i;

	i = 0;
	if (!paint)
		return (1);
	while (i < zone.h)
	{
		if (!(paint[i] = calloc(1, zone.w + 1)))
			return (1);
		memset(paint[i], zone.bc, zone.w);
		++i;
	}
	paint[i] = 0;
	return (0);
}

int		drawing(FILE *stream, t_zone zone)
{
	int		ret;
	t_draw	draw;
	char	n;
	char	**paint;

	paint = (char **)malloc(sizeof(char *) * (zone.h + 1));
	if (init_paint(paint, zone) == 1)
		return (ft_freetab(paint));
	init_draw(&draw);
	ret = fscanf(stream, "%c%c %f %f %f %c", &n, &draw.type, &draw.x,
	&draw.y, &draw.rad, &draw.c);
	while (ret == 6)
	{
		if ((draw.type != 'c' && draw.type != 'C') || n != '\n' ||
		draw.rad <= 0.0 || draw.c == 0 || draw.c == '\n')
			return (ft_freetab(paint));
		draw_circle(draw, zone, paint);
		init_draw(&draw);
		ret = fscanf(stream, "%c%c %f %f %f %c", &n, &draw.type, &draw.x,
		&draw.y, &draw.rad, &draw.c);
	}
	if (draw.type != '\0' && ret != -1)
		return (ft_freetab(paint));
	ft_puttab(paint);
	ft_freetab(paint);
	return (0);
}

int		get_zone(FILE *stream, t_zone *zone)
{
	size_t	ret;

	ret = fscanf(stream, "%d %d %c", &zone->w, &zone->h, &zone->bc);
	if (ret != 3)
		return (1);
	if (zone->w <= 0 || zone->w > 300 || zone->h <= 0 || zone->h > 300)
		return (1);
	return (0);
}

int 	main(int ac, char **av)
{
	FILE	*stream;
	t_zone	zone;

	if (ac != 2)
		return (ft_putstr("Error: argument\n"));
	if (!(stream = fopen(av[1], "r")))
		return (ft_putstr("Error: Operation file corrupted\n"));
	if (get_zone(stream, &zone) == 1)
		return (ft_putstr("Error: Operation file corrupted\n"));
	if (drawing(stream, zone) == 1)
		return (ft_putstr("Error: Operation file corrupted\n"));
	fclose(stream);
	return (0);
}
