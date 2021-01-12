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

int		get_zone(FILE *stream, t_zone *zone)
{
	size_t	ret;

	ret = fscanf(stream, "%d %d %c", &zone->w, &zone->h, &zone->bc);
	if (ret != 3 && ret != 0)
		return (ft_putstr("Error: Operation file corrupted\n"));
	if (zone->w <= 0 || zone->w > 300 || zone->h <= 0 || zone->h > 300)
		return (1);
	return (0);
}

int		is_border(float xa, float ya, float xb, float yb, float rad)
{
	float	d;

	d = sqrtf((xa - xb) * (xa - xb) + (ya - yb) * (ya - yb));
	if (d <= rad)
	{
		if ((rad - d) <= 1.0)
			return (1);
		else
			return (0);
	}
	return (-1);
	
}

int		draw_cercle(t_draw draw, t_zone zone, char **paint)
{
	int	i;
	int	j;

	i = 0;
	while (i < zone.h)
	{
		j = 0;
		while (j < zone.w)
		{
			if (is_border(i, j, draw.x, draw.y, draw.rad) == 1 ||
			(draw.type == 'C' && is_border(i, j, draw.x, draw.y, draw.rad) != -1))
				paint[i][j] = draw.c;
			++j;
		}
		++i;
	}
	return (0);
}

int		drawing(FILE *stream, t_zone zone)
{
	size_t	ret;
	t_draw	draw;
	char	n;
	char	**paint;

	ret = fscanf(stream, "%c%c %f %f %f %c", &n, &draw.type, &draw.x,
	&draw.y, &draw.rad, &draw.c);
	int i = 0;
	paint = (char **)malloc(sizeof(char *) * zone.w);
	while (i < zone.h)
	{
		paint[i] = calloc(1, zone.w);
		memset(paint[i], zone.bc, zone.w);
		++i;
	}
	while (ret == 6)
	{
		if ((draw.type != 'c' && draw.type != 'C')
		|| n != '\n' || draw.rad == 0)
			return (1);
		draw_cercle(draw, zone, paint);
		ret = fscanf(stream, "%c%c %f %f %f %c", &n, &draw.type, &draw.x,
		&draw.y, &draw.rad, &draw.c);
	}
	i = 0;
	if (draw.type != 'C' && draw.type != 'c')
		return (1);
	while (i < zone.h)
	{
		ft_putstr(paint[i]);
		write(1, "\n", 1);
		free(paint[i]);
		++i;
	}
	free(paint);
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
		return (1);
	return (drawing(stream, zone));
}