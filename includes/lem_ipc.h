#ifndef LEM_IPC_H
# define LEM_IPC_H

# include "../libft/ft_printf/ft_printf.h"
# include "../libft/libft.h"
# include <sys/ipc.h>
# include <sys/shm.h>
# include <sys/types.h>
# include <time.h>

# define MAP_WIDTH 10
# define MAP_HEIGHT 10
# define MAP_SIZE (MAP_WIDTH * MAP_HEIGHT)

# define SHM_KEY 0x1234

typedef struct s_player
{
	int	x;
	int	y;
	int	team_id;
}		t_player;

typedef struct s_ipc
{
	int	*map;
	int	shmid;
	int	is_creator;
}		t_ipc;

// MAIN
int		place_player(int *map, t_player *player);

// UTILS
int		get_index(int x, int y);
int		get_cell(int *map, int x, int y);
void	set_cell(int *map, int x, int y, int value);
void	display_map(int *map);
void	cleanup(t_ipc *ipc);

#endif
