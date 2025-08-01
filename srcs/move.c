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

static t_point	find_average_position(t_point *positions, int count)
{
	int	sum_x;
	int	sum_y;
	int	i;

	sum_x = 0;
	sum_y = 0;
	i = 0;
	while (i < count)
	{
		sum_x += positions[i].x;
		sum_y += positions[i].y;
		i++;
	}
	return ((t_point){sum_x / count, sum_y / count});
}

int	move_player(t_ipc *ipc, t_player *player)
{
	t_point	target;
	int		ally_count;
	t_msg	buffer[10];
	t_point	positions[10];
	int		result;

	ally_count = received_team_position(ipc, player->team_id, buffer, 10);
	if (ally_count > 0)
	{
		for (int i = 0; i < ally_count; i++)
		{
			positions[i].x = buffer[i].x;
			positions[i].y = buffer[i].y;
		}
		target = find_average_position(positions, ally_count);
		result = move_towards(ipc, player, target);
	}
	else
		result = random_move(ipc, player);
	send_player_position(ipc, player);
	return (result);
}
