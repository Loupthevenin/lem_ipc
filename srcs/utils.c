#include "../includes/lem_ipc.h"

// Map
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

void	display_map(int *map)
{
	int	y;
	int	x;
	int	val;

	y = 0;
	// TODO: Mettre en place le visual avec le clear en argv ? bonus ?;
	/* system("clear"); */
	ft_printf("   ");
	x = 0;
	while (x < MAP_WIDTH)
		ft_printf("%d ", x++);
	ft_printf("\n");
	while (y < MAP_HEIGHT)
	{
		x = 0;
		ft_printf("%d  ", y);
		while (x < MAP_WIDTH)
		{
			val = get_cell(map, x, y);
			if (val == 0)
				ft_printf(". ");
			else
			{
				switch (val)
				{
				case 1:
					ft_printf(RED "%d " RESET, val);
					break ;
				case 2:
					ft_printf(GREEN "%d " RESET, val);
					break ;
				case 3:
					ft_printf(YELLOW "%d " RESET, val);
					break ;
				case 4:
					ft_printf(BLUE "%d " RESET, val);
					break ;
				default:
					ft_printf("%d ", val);
				}
			}
			x++;
		}
		ft_printf("\n");
		y++;
	}
}

// Semaphore
int	create_semaphore(key_t key, int is_creator)
{
	int			semid;
	union semun	arg;

	semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
	if (semid != -1)
	{
		if (is_creator)
		{
			arg.val = 1;
			if (semctl(semid, 0, SETVAL, arg) == -1)
			{
				ft_putstr_fd("Error: semctl SETVAL\n", 2);
				return (-1);
			}
		}
	}
	else
	{
		// S'il existe déjà, on le récupère
		semid = semget(key, 1, 0666);
		if (semid == -1)
		{
			ft_putstr_fd("Error: semget fallback\n", 2);
			return (-1);
		}
	}
	return (semid);
}

void	semaphore_wait(int semid)
{
	struct sembuf	sb;

	sb.sem_num = 0;
	sb.sem_op = -1;
	sb.sem_flg = 0;
	if (semop(semid, &sb, 1) == -1)
	{
		ft_putstr_fd("Error: semop wait\n", 2);
		exit(EXIT_FAILURE);
	}
}

void	semaphore_signal(int semid)
{
	struct sembuf	sb;

	sb.sem_num = 0;
	sb.sem_op = 1;
	sb.sem_flg = 0;
	if (semop(semid, &sb, 1) == -1)
	{
		ft_putstr_fd("Error: semop signal", 2);
		exit(EXIT_FAILURE);
	}
}

void	destroy_ipc_resources(int shmid, int semid, int msgid)
{
	if (shmid != -1)
		if (shmctl(shmid, IPC_RMID, NULL) == -1)
			ft_putstr_fd("Error: shmctl IPC_RMID\n", 2);
	if (semid != -1)
		if (semctl(semid, 0, IPC_RMID) == -1)
			ft_putstr_fd("Error: semctl IPC_RMID\n", 2);
	if (msgid != -1)
		if (msgctl(msgid, IPC_RMID, NULL) == -1)
			ft_putstr_fd("Error: msgctl IPC_RMID\n", 2);
}

// Other
int	is_number(char *str)
{
	int	i;

	i = 0;
	if (!str || !str[0])
		return (0);
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

void	handle_sigint(int sig)
{
	(void)sig;
	g_exit = 1;
}

void	safe_print(t_ipc *ipc, char *msg)
{
	semaphore_wait(ipc->semid);
	ft_printf("%s\n", msg);
	semaphore_signal(ipc->semid);
}

void	cleanup(t_ipc *ipc)
{
	int	alive_players;

	semaphore_wait(ipc->semid);
	alive_players = count_alive_players(ipc->map);
	ft_printf("alive_players: %d\n", alive_players);
	semaphore_signal(ipc->semid);
	if (ipc->map)
		shmdt(ipc->map);
	if (ipc->game_state)
		shmdt(ipc->game_state);
	if (alive_players == 0)
		destroy_ipc_resources(ipc->shmid, ipc->semid, ipc->msgid);
}
