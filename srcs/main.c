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

void	init_ipc(t_ipc *ipc)
{
	int	size;

	size = MAP_SIZE * sizeof(int);
	ipc->shmid = shmget(SHM_KEY, size, IPC_CREAT | IPC_EXCL | 0666);
	if (ipc->shmid == -1)
	{
		ipc->shmid = shmget(SHM_KEY, size, 0666);
		if (ipc->shmid == -1)
		{
			ft_putstr_fd("Error: shmget\n", 2);
			ipc->map = NULL;
			return ;
		}
		ipc->is_creator = 0;
	}
	else
		ipc->is_creator = 1;
	ipc->map = (int *)shmat(ipc->shmid, NULL, 0);
	if (ipc->map == (void *)-1)
	{
		ft_putstr_fd("Error: shmat\n", 2);
		ipc->map = NULL;
	}
}

int	main(int argc, char **argv)
{
	t_ipc		ipc;
	t_player	player;

	(void)argc;
	(void)argv;
	init_ipc(&ipc);
	player.team_id = 1;
	if (place_player(ipc.map, &player) != 0)
	{
		ft_putstr_fd("Error: no place for player\n", 2);
		cleanup(&ipc);
		return (1);
	}
	display_map(ipc.map);
	cleanup(&ipc);
	return (0);
}
