#include "../includes/lem_ipc.h"

static int	init_shared_memory(t_ipc *ipc)
{
	int	size;
	int	*shared_mem;

	size = (MAP_SIZE + 1) * sizeof(int); // + 1 pour le game_state
	ipc->shmid = shmget(SHM_KEY, size, IPC_CREAT | IPC_EXCL | 0666);
	if (ipc->shmid == -1)
	{
		ipc->shmid = shmget(SHM_KEY, size, 0666);
		if (ipc->shmid == -1)
			return (ft_putstr_fd("Error: shmget\n", 2), -1);
		ipc->is_creator = 0;
	}
	else
		ipc->is_creator = 1;
	shared_mem = (int *)shmat(ipc->shmid, NULL, 0);
	if (shared_mem == (void *)-1)
		return (ft_putstr_fd("Error: shmat\n", 2), -1);
	ipc->map = shared_mem;
	ipc->game_state = &shared_mem[MAP_SIZE];
	if (ipc->is_creator)
	{
		ft_memset(ipc->map, 0, MAP_SIZE * sizeof(int));
		*(ipc->game_state) = WAITING;
	}
	return (0);
}

static int	init_semaphore(t_ipc *ipc)
{
	ipc->semid = create_semaphore(SEM_KEY, ipc->is_creator);
	if (ipc->semid == -1)
		return (ft_putstr_fd("Error: failed to create semaphore\n", 2), -1);
	return (0);
}

static int	init_message_queue(t_ipc *ipc)
{
	ipc->msgid = msgget(MSG_KEY, IPC_CREAT | IPC_EXCL | 0666);
	if (ipc->msgid == -1)
	{
		ipc->msgid = msgget(MSG_KEY, 0666);
		if (ipc->msgid == -1)
			return (ft_putstr_fd("Error: msgget\n", 2), -1);
	}
	return (0);
}

void	init_ipc(t_ipc *ipc)
{
	ipc->map = NULL;
	ipc->game_state = NULL;
	ipc->shmid = -1;
	ipc->semid = -1;
	ipc->msgid = -1;
	ipc->is_creator = 0;
	if (init_shared_memory(ipc) == -1 || init_semaphore(ipc) == -1
		|| init_message_queue(ipc) == -1)
	{
		ipc->map = NULL;
		ipc->game_state = NULL;
	}
}
