#include "../includes/lem_ipc.h"

int	send_player_position(t_ipc *ipc, t_player *player)
{
	struct s_msg	msg;

	msg.mtype = player->team_id;
	msg.team_id = player->team_id;
	msg.x = player->x;
	msg.y = player->y;
	return (msgsnd(ipc->msgid, &msg, sizeof(t_msg) - sizeof(long), 0));
}

int	received_team_position(t_ipc *ipc, int team_id, t_msg *buffer, int max)
{
	int		received;
	ssize_t	bytes;

	received = 0;
	while (received < max)
	{
		bytes = msgrcv(ipc->msgid, &buffer[received], sizeof(t_msg)
				- sizeof(long), team_id, IPC_NOWAIT);
		if (bytes == -1)
			break ;
		received++;
	}
	return (received);
}
