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

static int	get_valid_moves(int *map, t_point origin, t_point *options)
{
	int		count;
	t_point	next;

	count = 0;
	for (int i = 0; i < 4; i++)
	{
		next = get_adjacent_position(origin, i);
		if (is_valid_move(map, next.x, next.y))
			options[count++] = next;
	}
	return (count);
}

static void	update_player_position(t_ipc *ipc, t_player *player,
		t_point new_pos)
{
	// Effacer l'ancienne position
	set_cell(ipc->map, player->x, player->y, 0);
	// Move
	set_cell(ipc->map, new_pos.x, new_pos.y, player->team_id);
	player->x = new_pos.x;
	player->y = new_pos.y;
}

static int	random_move(t_ipc *ipc, t_player *player)
{
	t_point	options[4];
	int		count;
	t_point	chosen;

	count = get_valid_moves(ipc->map, (t_point){player->x, player->y}, options);
	if (count == 0)
		return (0);
	chosen = options[rand() % count];
	update_player_position(ipc, player, chosen);
	return (1);
}

static int	move_towards(t_ipc *ipc, t_player *player, t_point target)
{
	t_point	options[4];
	t_point	best;
	int		count;
	int		min_dist;
	int		dist;

	count = get_valid_moves(ipc->map, (t_point){player->x, player->y}, options);
	if (count == 0)
		return (0);
	min_dist = 100000;
	best = options[0];
	for (int i = 0; i < count; i++)
	{
		dist = abs(options[i].x - target.x) + abs(options[i].y - target.y);
		if (dist < min_dist)
		{
			min_dist = dist;
			best = options[i];
		}
	}
	update_player_position(ipc, player, best);
	return (1);
}

static t_point	find_closest_teammate(t_ipc *ipc, t_player *player)
{
	int		min_dist;
	int		dist;
	int		x;
	int		y;
	t_point	closest;

	min_dist = 100000;
	closest.x = -1;
	closest.y = -1;
	y = 0;
	while (y < MAP_HEIGHT)
	{
		x = 0;
		while (x < MAP_WIDTH)
		{
			if (get_cell(ipc->map, x, y) == player->team_id && !(x == player->x
					&& y == player->y))
			{
				dist = abs(x - player->x) + abs(y - player->y);
				if (dist < min_dist)
				{
					min_dist = dist;
					closest = (t_point){x, y};
				}
			}
			x++;
		}
		y++;
	}
	return (closest);
}

int	move_player(t_ipc *ipc, t_player *player)
{
	t_point	target;
	int		result;

	target = find_closest_teammate(ipc, player);
	if (target.x == -1 && target.y == -1)
		result = random_move(ipc, player);
	else
		result = move_towards(ipc, player, target);
	send_player_position(ipc, player);
	return (result);
}
