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

int	place_player(int *map, t_player *player)
{
	int	tries;
	int	x;
	int	y;

	srand(getpid());
	tries = 0;
	while (tries < 100)
	{
		x = rand() % MAP_WIDTH;
		y = rand() % MAP_HEIGHT;
		if (get_cell(map, x, y) == 0)
		{
			set_cell(map, x, y, player->team_id);
			player->x = x;
			player->y = y;
			return (0);
		}
		tries++;
	}
	return (1);
}

int	main(int argc, char **argv)
{
	int			*map;
	int			size;
	t_player	player;

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
	player.team_id = 1;
	if (place_player(map, &player) != 0)
	{
		ft_putstr_fd("Error: no place for player", 2);
		free(map);
		return (1);
	}
	display_map(map);
	free(map);
	return (0);
}
