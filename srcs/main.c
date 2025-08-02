#include "../includes/lem_ipc.h"

volatile sig_atomic_t	g_exit = 0;

t_args	parse_args(int argc, char **argv)
{
	t_args	args;
	int		i;

	args.visual = 0;
	args.team_id = -1;
	if (argc > 3)
	{
		ft_putstr_fd("Usage: ./lemipc <team_id>\n", 2);
		exit(EXIT_FAILURE);
	}
	i = 1;
	while (i < argc)
	{
		if (ft_strcmp(argv[i], "-v") == 0)
			args.visual = 1;
		else if (is_number(argv[i]))
		{
			if (args.team_id != -1)
			{
				ft_putstr_fd("Error: Multiple team IDs provided\n", 2);
				exit(EXIT_FAILURE);
			}
			args.team_id = ft_atoi(argv[i]);
		}
		else
		{
			ft_putstr_fd("Error: Invalid argument\n", 2);
			exit(EXIT_FAILURE);
		}
		i++;
	}
	if (args.team_id == -1)
		args.team_id = (rand() % 9) + 1;
	if (args.team_id < 1 || args.team_id > 9)
	{
		ft_putstr_fd("Error: team_id must be between 1 and 9\n", 2);
		exit(EXIT_FAILURE);
	}
	return (args);
}

// TODO: Actuellement l'ordre est random mais on peut placer les joueurs de manière a ce qu'il n'y ai pas de collision dès le depart;
int	place_player(int *map, t_player *player)
{
	int	tries;
	int	x;
	int	y;

	tries = 0;
	while (tries < MAP_SIZE)
	{
		x = rand() % MAP_WIDTH;
		y = rand() % MAP_HEIGHT;
		if (get_cell(map, x, y) == 0)
		{
			set_cell(map, x, y, player->team_id);
			player->x = x;
			player->y = y;
			player->last_pos.x = player->x;
			player->last_pos.y = player->y;
			return (0);
		}
		tries++;
	}
	return (-1);
}

int	main(int argc, char **argv)
{
	t_ipc		ipc;
	t_player	player;
	t_args		args;

	srand(getpid());
	signal(SIGINT, handle_sigint);
	args = parse_args(argc, argv);
	player.team_id = args.team_id;
	player.player_id = getpid();
	player.alive = 1;
	if (g_exit)
		return (0);
	init_ipc(&ipc);
	if (!ipc.map)
		return (1);
	semaphore_wait(ipc.semid);
	if (place_player(ipc.map, &player) != 0)
	{
		ft_putstr_fd("Error: no place for player\n", 1);
		semaphore_signal(ipc.semid);
		cleanup(&ipc);
		return (1);
	}
	semaphore_signal(ipc.semid);
	if (ipc.is_creator)
		wait_for_teams(&ipc, &player, MIN_TEAMS_TO_START);
	else
		wait_for_start(&ipc, &player);
	if (!g_exit)
		game_loop(&ipc, &player, &args);
	else
	{
		semaphore_wait(ipc.semid);
		quit_game_sigint(&ipc, &player);
	}
	cleanup(&ipc);
	return (0);
}
