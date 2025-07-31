#include "../includes/lem_ipc.h"

static int	is_valid_move(int *map, int x, int y)
{
	if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT)
		return (0);
	if (get_cell(map, x, y) != 0)
		return (0);
	return (1);
}

static t_point	get_adjacent_position(t_point origin, int direction)
{
	if (direction == TOP)
		return ((t_point){origin.x, origin.y - 1});
	if (direction == BOTTOM)
		return ((t_point){origin.x, origin.y + 1});
	if (direction == LEFT)
		return ((t_point){origin.x - 1, origin.y});
	if (direction == RIGHT)
		return ((t_point){origin.x + 1, origin.y});
	return (origin);
}

int	move_player(int *map, t_player *player)
{
	t_point	options[4];
	int		count;
	t_point	next;
	int		chosen;

	count = 0;
	for (int i = 0; i < 4; i++)
	{
		next = get_adjacent_position((t_point){player->x, player->y}, i);
		if (is_valid_move(map, next.x, next.y))
			options[count++] = next;
	}
	if (count == 0)
		return (0);
	chosen = rand() % count;
	// Effacer l'ancienne position;
	set_cell(map, player->x, player->y, 0);
	// Move;
	set_cell(map, options[chosen].x, options[chosen].y, player->team_id);
	player->x = options[chosen].x;
	player->y = options[chosen].y;
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
