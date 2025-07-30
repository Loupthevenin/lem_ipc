#include "../includes/lem_ipc.h"

int	get_index(int x, int y)
{
	return (y * MAP_WIDTH + x);
}

int	get_cell(int *map, int x, int y)
{
	return (map[get_index(x, y)]);
}

void	set_cell(int *map, int x, int y, int value)
{
	map[get_index(x, y)] = value;
}

void	cleanup(t_ipc *ipc)
{
	if (ipc->map)
		shmdt(ipc->map);
	if (ipc->is_creator && ipc->shmid != -1)
	{
		if (shmctl(ipc->shmid, IPC_RMID, NULL) == -1)
			ft_putstr_fd("Error: shmctl\n", 2);
	}
}
