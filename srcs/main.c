#include "../includes/lem_ipc.h"

volatile sig_atomic_t	g_exit_requested = 0;

int	parse_team_id(int argc, char **argv)
{
	int	team_id;

	if (argc > 2)
	{
		ft_putstr_fd("Usage: ./lemipc <team_id>\n", 2);
		exit(EXIT_FAILURE);
	}
	if (argc == 2)
	{
		if (!is_number(argv[1]))
		{
			ft_putstr_fd("Error: team_id must be a number\n", 2);
			exit(EXIT_FAILURE);
		}
		team_id = ft_atoi(argv[1]);
		if (team_id < 1 || team_id > 9)
		{
			ft_putstr_fd("Error: team_id muste be between 1 and 9\n", 2);
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		team_id = (rand() % 9) + 1;
		ft_printf("Random team assigned: %d\n", team_id);
	}
	return (team_id);
}

int	place_player(int *map, t_player *player)
{
	int	tries;
	int	x;
	int	y;

	tries = 0;
	while (tries < 100)
	{
		x = rand() % MAP_WIDTH;
		y = rand() % MAP_HEIGHT;
		if (get_cell(map, x, y) == 0)
		{
			set_cell(map, x, y, player->team_id);
			player->x = x;
			player->y = y;
			return (0);
		}
		tries++;
	}
	return (1);
}

void	init_ipc(t_ipc *ipc)
{
	int	size;
	int	*shared_mem;

	size = (MAP_SIZE + 1) * sizeof(int); // + 1 pour le game_state
	ipc->shmid = shmget(SHM_KEY, size, IPC_CREAT | IPC_EXCL | 0666);
	if (ipc->shmid == -1)
	{
		ipc->shmid = shmget(SHM_KEY, size, 0666);
		if (ipc->shmid == -1)
		{
			ft_putstr_fd("Error: shmget\n", 2);
			ipc->map = NULL;
			ipc->game_state = NULL;
			return ;
		}
		ipc->is_creator = 0;
	}
	else
		ipc->is_creator = 1;
	shared_mem = (int *)shmat(ipc->shmid, NULL, 0);
	if (shared_mem == (void *)-1)
	{
		ft_putstr_fd("Error: shmat\n", 2);
		ipc->map = NULL;
		ipc->game_state = NULL;
		return ;
	}
	ipc->map = shared_mem;
	ipc->game_state = &shared_mem[MAP_SIZE];
	if (ipc->is_creator)
	{
		ft_memset(ipc->map, 0, MAP_SIZE * sizeof(int));
		*(ipc->game_state) = WAITING;
	}
	ipc->semid = create_semaphore(SEM_KEY, ipc->is_creator);
	if (ipc->semid == -1)
	{
		ft_putstr_fd("Error: failed to create semaphore\n", 2);
		ipc->map = NULL;
		ipc->game_state = NULL;
		return ;
	}
}

int	main(int argc, char **argv)
{
	t_ipc		ipc;
	t_player	player;

	srand(getpid());
	signal(SIGINT, handle_sigint);
	player.team_id = parse_team_id(argc, argv);
	player.player_id = getpid();
	player.alive = 1;
	init_ipc(&ipc);
	if (!ipc.map)
		return (1);
	semaphore_wait(ipc.semid);
	if (place_player(ipc.map, &player) != 0)
	{
		ft_putstr_fd("Error: no place for player\n", 2);
		semaphore_signal(ipc.semid);
		cleanup(&ipc);
		return (1);
	}
	semaphore_signal(ipc.semid);
	if (ipc.is_creator)
		wait_for_teams(&ipc, MIN_TEAMS_TO_START);
	else
		wait_for_start(&ipc);
	if (!g_exit_requested)
		game_loop(&ipc, &player);
	cleanup(&ipc);
	return (0);
}
