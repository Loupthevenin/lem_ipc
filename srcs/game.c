#include "../includes/lem_ipc.h"

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

static void	print_team_win(int team_id)
{
	const char	*color;

	switch (team_id)
	{
	case 1:
		color = RED;
		break ;
	case 2:
		color = GREEN;
		break ;
	case 3:
		color = YELLOW;
		break ;
	case 4:
		color = BLUE;
		break ;
	default:
		color = RESET;
		break ;
	}
	ft_printf("%sTeam %d wins the game!%s\n", color, team_id, RESET);
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

void	wait_for_teams(t_ipc *ipc, t_player *player, int min_teams)
{
	int	alive_teams;

	semaphore_wait(ipc->semid);
	ft_printf("Waiting for at least %d teams to join...\n", min_teams);
	semaphore_signal(ipc->semid);
	while (!g_exit)
	{
		semaphore_wait(ipc->semid);
		alive_teams = count_alive_teams(ipc->map);
		ft_printf("Teams joined: %d/%d\n", alive_teams, min_teams);
		semaphore_signal(ipc->semid);
		if (alive_teams >= min_teams)
		{
			semaphore_wait(ipc->semid);
			*(ipc->game_state) = START;
			ft_printf("Enough teams joined (%d)\n",
						alive_teams);
			semaphore_signal(ipc->semid);
			break ;
		}
		sleep(1);
	}
	if (g_exit)
	{
		semaphore_wait(ipc->semid);
		kill_player(ipc, player, "");
		semaphore_signal(ipc->semid);
	}
}

void	wait_for_start(t_ipc *ipc, t_player *player)
{
	int	state;

	semaphore_wait(ipc->semid);
	ft_printf("Waiting for the game to start...\n");
	semaphore_signal(ipc->semid);
	while (!g_exit)
	{
		semaphore_wait(ipc->semid);
		state = *(ipc->game_state);
		ft_printf("Game state: %s\n",
					state == WAITING ? "Waiting" : "Starting");
		semaphore_signal(ipc->semid);
		if (state != WAITING)
			break ;
		sleep(1);
	}
	if (g_exit)
	{
		semaphore_wait(ipc->semid);
		kill_player(ipc, player, "");
		semaphore_signal(ipc->semid);
	}
}

void	quit_game_sigint(t_ipc *ipc, t_player *player)
{
	if (count_alive_players(ipc->map) > 1)
	{
		kill_player(ipc, player, "SIGINT");
		if (ipc->map)
			shmdt(ipc->map);
		if (ipc->game_state)
			shmdt(ipc->game_state);
		semaphore_signal(ipc->semid);
		exit(130);
	}
	if (ipc->map)
		shmdt(ipc->map);
	if (ipc->game_state)
		shmdt(ipc->game_state);
	semaphore_signal(ipc->semid);
	destroy_ipc_resources(ipc->shmid, ipc->semid, ipc->msgid);
	exit(130);
}

static int	check_exit_requested(t_ipc *ipc, t_player *player)
{
	if (g_exit && player->alive)
	{
		quit_game_sigint(ipc, player);
		return (1);
	}
	return (0);
}

static int	check_game_end(t_ipc *ipc, t_player *player)
{
	if (*(ipc->game_state) == END)
	{
		kill_player(ipc, player, "game ended");
		return (1);
	}
	return (0);
}

static int	check_combat(t_ipc *ipc, t_player *player)
{
	if (count_adjacent_enemies(ipc->map, player) >= 2)
	{
		kill_player(ipc, player, "in combat");
		return (1);
	}
	return (0);
}

static int	check_win_condition(t_ipc *ipc, t_player *player)
{
	if (count_alive_teams(ipc->map) <= 1)
	{
		print_team_win(player->team_id);
		*(ipc->game_state) = END;
		return (1);
	}
	return (0);
}

void	game_loop(t_ipc *ipc, t_player *player, t_args *args)
{
	while (player->alive)
	{
		semaphore_wait(ipc->semid);
		if (check_exit_requested(ipc, player) || check_game_end(ipc, player)
			|| check_combat(ipc, player))
		{
			semaphore_signal(ipc->semid);
			break ;
		}
		move_player(ipc, player);
		display_map(ipc->map, args);
		if (check_win_condition(ipc, player))
		{
			semaphore_signal(ipc->semid);
			break ;
		}
		semaphore_signal(ipc->semid);
		sleep(1);
	}
}
