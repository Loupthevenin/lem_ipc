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

static void	kill_player(t_ipc *ipc, t_player *player, const char *reason)
{
	set_cell(ipc->map, player->x, player->y, 0);
	player->alive = 0;
	ft_printf("Player %d died %s at (%d, %d)\n",
				player->player_id,
				reason,
				player->x,
				player->y);
}

static int	count_adjacent_enemies(int *map, t_player *player)
{
	int		count;
	t_point	p;
	t_point	next;
	int		val;

	p.x = player->x;
	p.y = player->y;
	count = 0;
	for (int i = 0; i < 4; i++)
	{
		next = get_adjacent_position(p, i);
		if (next.x < 0 || next.x >= MAP_WIDTH || next.y < 0
			|| next.y >= MAP_HEIGHT)
			continue ;
		val = get_cell(map, next.x, next.y);
		if (val != 0 && val != player->team_id)
			count++;
	}
	return (count);
}

static int	count_alive_teams(int *map)
{
	int	teams[MAX_TEAMS + 1];
	int	count;
	int	val;

	count = 0;
	ft_memset(teams, 0, sizeof(int) * (MAX_TEAMS + 1));
	for (int y = 0; y < MAP_WIDTH; y++)
	{
		for (int x = 0; x < MAP_WIDTH; x++)
		{
			val = get_cell(map, x, y);
			if (val > 0 && teams[val] == 0)
			{
				teams[val] = 1;
				count++;
			}
		}
	}
	return (count);
}

static int	move_player(int *map, t_player *player)
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

int	count_alive_players(int *map)
{
	int	count;
	int	i;

	count = 0;
	i = 0;
	while (i < MAP_SIZE)
	{
		if (map[i] > 0)
			count++;
		i++;
	}
	return (count);
}

void	wait_for_teams(t_ipc *ipc, int min_teams)
{
	int	alive_teams;

	ft_printf("Waiting for at least %d teams to join...\n", min_teams);
	while (1)
	{
		semaphore_wait(ipc->semid);
		alive_teams = count_alive_teams(ipc->map);
		semaphore_signal(ipc->semid);
		ft_printf("\rTeams joined: %d/%d\n", alive_teams, min_teams);
		if (alive_teams >= min_teams)
		{
			semaphore_wait(ipc->semid);
			*(ipc->game_state) = START;
			semaphore_signal(ipc->semid);
			ft_printf("Enough teams joined (%d), starting the game!\n",
						alive_teams);
			break ;
		}
		sleep(1);
	}
}

void	wait_for_start(t_ipc *ipc)
{
	int	state;

	ft_printf("Waiting for the game to start...\n");
	while (1)
	{
		semaphore_wait(ipc->semid);
		state = *(ipc->game_state);
		semaphore_signal(ipc->semid);
		ft_printf("Game state: %s\n",
					state == WAITING ? "Waiting" : "Starting");
		if (state != WAITING)
		{
			ft_printf("Game started\n");
			break ;
		}
		sleep(1);
	}
}

void	game_loop(t_ipc *ipc, t_player *player)
{
	while (player->alive)
	{
		semaphore_wait(ipc->semid);
		if (count_adjacent_enemies(ipc->map, player) >= 2)
		{
			kill_player(ipc, player, "in combat");
			semaphore_signal(ipc->semid);
			break ;
		}
		move_player(ipc->map, player);
		display_map(ipc->map);
		if (count_alive_teams(ipc->map) <= 1)
		{
			ft_printf("Team %d wins the game!\n", player->team_id);
			semaphore_signal(ipc->semid);
			break ;
		}
		semaphore_signal(ipc->semid);
		sleep(1);
	}
}
