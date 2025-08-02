#ifndef LEM_IPC_H
# define LEM_IPC_H

# include "../libft/ft_printf/ft_printf.h"
# include "../libft/libft.h"
# include <signal.h>
# include <sys/ipc.h>
# include <sys/msg.h>
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

# define MIN_TEAMS_TO_START 2
# define MAX_TEAMS 9
# define HISTORY_SIZE 5

# define SHM_KEY 0x1234
# define SEM_KEY 0x1337
# define MSG_KEY 0x1558

# define RESET "\033[0m"
# define RED "\033[31m"
# define GREEN "\033[32m"
# define YELLOW "\033[33m"
# define BLUE "\033[34m"
# define MAGENTA "\033[35m"
# define CYAN "\033[36m"
# define WHITE "\033[37m"

typedef struct s_ipc
{
	int							*map;
	int							shmid;
	int							semid;
	int							msgid;
	int							is_creator;
	int							*game_state;
}								t_ipc;

typedef struct s_point
{
	int							x;
	int							y;
}								t_point;

typedef struct s_args
{
	int							team_id;
	int							visual;
}								t_args;

typedef struct s_player
{
	int							team_id;
	int							x;
	int							y;
	int							player_id;
	int							alive;
	t_point						history[HISTORY_SIZE];
	int							history_index;
}								t_player;

typedef struct s_msg
{
	long mtype; // Type du message pour filtrer
	int							team_id;
	int							x;
	int							y;
}								t_msg;

// Union nécessaire pour certaines opérations de sémaphore
union							semun
{
	int val;               // Valeur du sémaphore
	struct semid_ds *buf;  // Informations sur l'ensemble de sémaphores
	unsigned short *array; // Tableau de valeurs pour SETALL
	struct seminfo *__buf; // Infos internes (non utilisé)
};

extern volatile sig_atomic_t	g_exit;

// MAIN
void							init_ipc(t_ipc *ipc);
int								place_player(int *map, t_player *player);
void							wait_for_teams(t_ipc *ipc, t_player *player,
									int min_teams);
void							wait_for_start(t_ipc *ipc, t_player *player);
void							game_loop(t_ipc *ipc, t_player *player,
									t_args *args);
int								move_player(t_ipc *ipc, t_player *player);
void							quit_game_sigint(t_ipc *ipc, t_player *player);

// SEMAPHORE
int								create_semaphore(key_t key, int is_creator);
void							semaphore_wait(int semid);
void							semaphore_signal(int semid);

// MSGQ
int	send_player_position(t_ipc *ipc,
							t_player *player);
int								received_team_position(t_ipc *ipc, int team_id,
									t_msg *buffer, int max);

// UTILS
int								get_index(int x, int y);
int								get_cell(int *map, int x, int y);
void							set_cell(int *map, int x, int y, int value);
t_point	get_adjacent_position(t_point origin,
								int direction);
void							display_map(int *map, t_args *args);
void							safe_print(t_ipc *ipc, char *msg);
int								count_alive_players(int *map);
void							destroy_ipc_resources(int shmid, int semid,
									int msgid);
void							cleanup(t_ipc *ipc);

// Other
int								is_number(char *str);
void							handle_sigint(int sig);

#endif
