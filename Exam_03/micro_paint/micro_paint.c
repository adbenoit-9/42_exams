/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   micro_paint.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/13 13:41:08 by adbenoit          #+#    #+#             */
/*   Updated: 2021/01/14 12:29:24 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "micro_paint.h"

static int	init_paint(char **paint, t_zone zone)
{
	int	i;

	if (!paint)
		return (1);
	i = 0;
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

static int	is_in_rect(float x, float y, t_draw draw)
{
	float	xbr;
	float	ybr;

	xbr = draw.x + draw.w;
	ybr = draw.y + draw.h;
	if (x >= draw.x && x <= xbr && y >= draw.y && y <= ybr)
	{
		if (x - draw.x < 1.0 || xbr - x < 1.0 || y - draw.y < 1.0
		|| ybr - y < 1.0)
			return (0);
		return (1);
	}
	return (-1);
}

static void	draw_rect(t_zone zone, t_draw draw, char **paint)
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
			k = is_in_rect(j, i, draw);
			if (k == 0 || (k == 1 && draw.type == 'R'))
				paint[i][j] = draw.c;
			++j;
		}
		++i;
	}

}

static int	drawing(FILE *stream, t_zone zone)
{
	int		ret;
	char	**paint;
	t_draw	draw;
	char	n;

	paint = (char **)malloc(sizeof(char *) * (zone.h + 1));
	if (init_paint(paint, zone) == 1)
		return (ft_freetab(paint));
	draw.type = 0;
	while ((ret = fscanf(stream, "%c%c %f %f %f %f %c", &n, &draw.type,
	&draw.x, &draw.y, &draw.w, &draw.h, &draw.c)) == 7)
	{
		if ((draw.type != 'r' && draw.type != 'R') || draw.h <= 0.0
		|| draw.w <= 0.0 || n != '\n'|| draw.c == 0 || draw.c == '\n')
			return (ft_freetab(paint));
		draw_rect(zone, draw, paint);
		draw.type = 0;
	}
	if (draw.type != 0 && ret != -1)
		return (ft_freetab(paint));
	ft_puttab(paint);
	ft_freetab(paint);
	return (0);
}

static int	get_info(FILE *stream, t_zone *zone)
{
	int	ret;

	ret = fscanf(stream, "%d %d %c", &zone->w, &zone->h, &zone->bc);
	if (zone->h <= 0 || zone->h > 300 || zone->w <= 0 || zone->w > 300
	|| zone->bc == 0 || zone->bc == '\n')
		return (1);
	return (0);
}

int 		main(int ac, char **av)
{
	FILE	*stream;
	t_zone	zone;

	if (ac != 2)
		return (ft_putstr("Error: argument\n"));
	if(!(stream = fopen(av[1], "r")))
		return (ft_putstr("Error: Operation file corrupted\n"));
	if (get_info(stream, &zone) == 1)
		return (ft_putstr("Error: Operation file corrupted\n"));
	if (drawing(stream, zone) == 1)
		return (ft_putstr("Error: Operation file corrupted\n"));
	fclose(stream);
	return (0);
}