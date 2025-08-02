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
	player->last_pos.x = player->x;
	player->last_pos.y = player->y;
}

static int	random_move(t_ipc *ipc, t_player *player)
{
	t_point	options[4];
	int		count;
	t_point	chosen;
	int		i;

	count = get_valid_moves(ipc->map, (t_point){player->x, player->y}, options);
	if (count == 0)
		return (0);
	i = 0;
	while (i < count)
	{
		if (options[i].x != player->last_pos.x
			|| options[i].y != player->last_pos.y)
		{
			chosen = options[i];
			break ;
		}
		i++;
	}
	if (i == count)
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
		// Empèche le retour à la case précédente
		if (options[i].x == player->last_pos.x
			&& options[i].y == player->last_pos.y)
			continue ;
		dist = abs(options[i].x - target.x) + abs(options[i].y - target.y);
		if (dist < min_dist)
		{
			min_dist = dist;
			best = options[i];
		}
	}
	// Aucun meilleur choix que revenir
	if (best.x == player->last_pos.x && best.y == player->last_pos.y)
		best = options[0];
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

static float	group_cohesion(t_point *positions, int count, t_point center)
{
	int		i;
	float	score;
	int		dist;

	i = 0;
	score = 0.0f;
	while (i < count)
	{
		dist = abs(positions[i].x - center.x) + abs(positions[i].y - center.y);
		if (dist == 0)
			score += 1.0f;
		else
			score += 1.0f / (float)dist;
		i++;
	}
	return (score);
}

static t_point	find_closest_enemy(t_ipc *ipc, t_player *player)
{
	t_point	closest;
	int		min_dist;
	int		cell;
	int		dist;

	min_dist = 100000;
	closest.x = -1;
	closest.y = -1;
	for (int y = 0; y < MAP_HEIGHT; y++)
	{
		for (int x = 0; x < MAP_WIDTH; x++)
		{
			cell = get_cell(ipc->map, x, y);
			if (cell != 0 && cell != player->team_id)
			{
				dist = abs(player->x - x) + abs(player->y - y);
				if (dist < min_dist)
				{
					min_dist = dist;
					closest.x = x;
					closest.y = y;
				}
			}
		}
	}
	return (closest);
}

int	move_player(t_ipc *ipc, t_player *player)
{
	t_point	target;
	t_point	enemy;
	int		ally_count;
	t_msg	buffer[10];
	t_point	positions[10];
	int		result;
	float	cohesion;

	ally_count = received_team_position(ipc, player->team_id, buffer, 10);
	if (ally_count > 0)
	{
		for (int i = 0; i < ally_count; i++)
		{
			positions[i].x = buffer[i].x;
			positions[i].y = buffer[i].y;
		}
		target = find_average_position(positions, ally_count);
		cohesion = group_cohesion(positions, ally_count, target);
		if (cohesion >= 1.5f)
		{
			enemy = find_closest_enemy(ipc, player);
			if (enemy.x != -1 && enemy.y != -1)
				result = move_towards(ipc, player, enemy);
			else
				result = random_move(ipc, player);
		}
		else
			result = move_towards(ipc, player, target);
	}
	else
		result = random_move(ipc, player);
	send_player_position(ipc, player);
	return (result);
}
