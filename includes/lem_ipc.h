#ifndef LEM_IPC_H
# define LEM_IPC_H

# include "../libft/ft_printf/ft_printf.h"
# include "../libft/libft.h"
# include <sys/ipc.h>
# include <sys/sem.h>
# include <sys/shm.h>
# include <sys/types.h>
# include <time.h>

# define MAP_WIDTH 10
# define MAP_HEIGHT 10
# define MAP_SIZE (MAP_WIDTH * MAP_HEIGHT)

# define TOP 0
# define BOTTOM 1
# define LEFT 2
# define RIGHT 3

# define WAITING 0
# define START 1
# define END 2

# define MAX_TEAMS 9

# define SHM_KEY 0x1234
# define SEM_KEY 0x1337

typedef struct s_player
{
	int				team_id;
	int				x;
	int				y;
	int				player_id;
	int				alive;
}					t_player;

typedef struct s_ipc
{
	int				*map;
	int				shmid;
	int				semid;
	int				is_creator;
	int				*game_state;
}					t_ipc;

typedef struct s_point
{
	int				x;
	int				y;
}					t_point;

union				semun
{
	int				val;
	struct semid_ds	*buf;
	unsigned short	*array;
	struct seminfo	*__buf;
};

// MAIN
int					place_player(int *map, t_player *player);
void				wait_for_teams(t_ipc *ipc, int min_teams);
void				wait_for_start(t_ipc *ipc);
void				game_loop(t_ipc *ipc, t_player *player);

// SEMAPHORE
int					create_semaphore(key_t key, int is_creator);
void				semaphore_wait(int semid);
void				semaphore_signal(int semid);
void				destroy_semaphore(int semid);

// UTILS
int					get_index(int x, int y);
int					get_cell(int *map, int x, int y);
void				set_cell(int *map, int x, int y, int value);
void				display_map(int *map);
void				cleanup(t_ipc *ipc);

// Other
int					is_number(char *str);

#endif
