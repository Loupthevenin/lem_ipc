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
