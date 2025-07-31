#include "../includes/lem_ipc.h"

static int	is_valid_move(int *map, int x, int y)
{
	if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT)
		return (0);
	if (get_cell(map, x, y) != 0)
		return (0);
	return (1);
}

int	move_player(int *map, t_player *player)
{
	int	options_x[4];
	int	options_y[4];
	int	count;
	int	new_x;
	int	new_y;
	int	chosen;

	count = 0;
	for (int i = 0; i < 4; i++)
	{
		new_x = player->x;
		new_y = player->y;
		if (i == 0)
			new_y -= 1;
		else if (i == 1)
			new_y += 1;
		else if (i == 2)
			new_x -= 1;
		else if (i == 3)
			new_x += 1;
		if (is_valid_move(map, new_x, new_y))
		{
			options_x[count] = new_x;
			options_y[count] = new_y;
			count++;
		}
	}
	if (count == 0)
		return (0);
	chosen = rand() % count;
	set_cell(map, player->x, player->y, 0);
	set_cell(map, options_x[chosen], options_y[chosen], player->team_id);
	player->x = options_x[chosen];
	player->y = options_y[chosen];
	return (1);
}

void	game_loop(t_ipc *ipc, t_player *player)
{
	int	turns;

	turns = 0;
	(void)player;
	while (player->alive)
	{
		semaphore_wait(ipc->semid);
		move_player(ipc->map, player);
		display_map(ipc->map);
		semaphore_signal(ipc->semid);
		sleep(1);
		turns++;
		// Simulation mort après 10 tours;
		if (turns == 10)
		{
			player->alive = 0;
			set_cell(ipc->map, player->x, player->y, 0);
			ft_printf("Player %d died at (%d, %d)\n", player->player_id,
					player->x, player->y);
		}
	}
}
