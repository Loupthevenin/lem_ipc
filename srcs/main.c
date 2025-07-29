#include "../includes/lem_ipc.h"

void	display_map(int *map)
{
	int	y;
	int	x;
	int	val;

	y = 0;
	while (y < MAP_HEIGHT)
	{
		x = 0;
		while (x < MAP_WIDTH)
		{
			val = get_cell(map, x, y);
			if (val == 0)
				ft_printf(".");
			else
				ft_printf("%d", val);
			x++;
		}
		ft_printf("\n");
		y++;
	}
}

int	main(int argc, char **argv)
{
	int	*map;
	int	size;

	(void)argc;
	(void)argv;
	size = sizeof(int) * MAP_SIZE;
	map = malloc(size);
	if (!map)
	{
		ft_putstr_fd("Error: malloc", 2);
		return (1);
	}
	ft_memset(map, 0, size);
	map[get_index(2, 3)] = 1;
	map[get_index(5, 5)] = 2;
	map[get_index(0, 0)] = 1;
	map[get_index(9, 9)] = 3;
	display_map(map);
	free(map);
	return (0);
}
