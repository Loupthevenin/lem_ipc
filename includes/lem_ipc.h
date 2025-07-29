#ifndef LEM_IPC_H
# define LEM_IPC_H

# include "../libft/ft_printf/ft_printf.h"
# include "../libft/libft.h"
# include <time.h>

# define MAP_WIDTH 10
# define MAP_HEIGHT 10
# define MAP_SIZE (MAP_WIDTH * MAP_HEIGHT)

typedef struct s_player
{
	int	x;
	int	y;
	int	team_id;
}		t_player;

// MAIN

// UTILS
int		get_index(int x, int y);
int		get_cell(int *map, int x, int y);
void	set_cell(int *map, int x, int y, int value);

#endif
