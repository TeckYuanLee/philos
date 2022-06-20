#ifndef PHILO_H
# define PHILO_H

# include <stdio.h>
# include <stdlib.h>

# include <unistd.h>
# include <string.h>
# include <pthread.h>
# include <sys/time.h>
# include <stdint.h>
# include <stdbool.h>
# include <limits.h>


# define ERR_INPUT "./philo [philos] [die_ms] [eat_ms] [sleep_ms] *opt:[no_of_eat]\n"
# define ERR_ARGS "Error: Please provide valid arguments\n"
# define ERR_MS "=> Philos 1-200; die, eat, sleep >= 60; no_of_eat > 0\n"
# define ERR_GEN "Error\n"
# define MSG_FORK " has taken a fork"
# define MSG_EAT " is eating"
# define MSG_SLEEP " is sleeping"
# define MSG_THINK " is thinking"
# define MSG_DIED " died"
# define MSG_ENOUGH " has eaten enough"
# define LEFT 0
# define RIGHT 1

# define DEBUG 0



# define RED "\e[0;31m"
# define GRN "\e[0;32m"
# define YEL "\e[0;33m"
# define BLU "\e[0;34m"
# define MAG "\e[0;35m"
# define CYN "\e[0;36m"
# define WHT "\e[0;37m"

typedef enum e_msg_type {
	FORK = 0,
	EAT,
	SLEEP,
	THINK,
	DEAD,
	ENOUGH
}			t_msg_type;


typedef struct s_init
{
	bool		fork;
	bool		philos;
}				t_init;

typedef struct s_philo
{
	pthread_t		id;
	int				seat;
	bool			active;
	int				times_eaten;
	uintmax_t		last_ate_ms;
	uintmax_t		deadline;
	struct s_arg	*arg;
	pthread_mutex_t	eat_lock;
	pthread_mutex_t	*p_forks[2];
}					t_philo;

typedef struct s_arg
{
	int				philos;
	int				die_ms;
	int				eat_ms;
	int				sleep_ms;
	int				no_of_eat;
	int				full_philos;
	uintmax_t		start_ms;
	bool			is_dead;
	pthread_mutex_t	msg_lock;
	pthread_mutex_t	dead_lock;
	pthread_mutex_t	*forks;
	struct s_init	init;
}					t_arg;


// check.c
void		*philo_check(void *philo_arg);

// exit.c
int			clean_exit(t_arg *args, t_philo **philos);

// fork.c
void		philo_clean_forks(t_philo *philo);
int			philo_take_forks(t_philo *philo);

// init.c
int			init_philos(t_arg *args, t_philo **philos);
int			init_forks(t_arg *args);
void		init_args(t_arg *args);

// main.c
int			status_change_message(t_philo *philo, const char *message,
				t_msg_type type);

// philo_utils.c
int			lock_check(t_philo *philo, pthread_mutex_t *lock, const char *fn);
int			update_eat_time(t_philo *philo);
bool		has_eaten_enough(t_philo *philo);
bool		gameover(t_philo *philo);
void		u_sleep_better(uintmax_t usec);

// philo.c
void		*philo_do(void *philo_arg);
int	philo_eat(t_philo *philo);
int	philo_sleep(t_philo *philo);
int	philo_think(t_philo *philo);

// prog_utils.c
const char	*get_philo_colour(int philo_seat);
int			ft_strcmp(void *s1, void *s2);
int			ft_atoi(const char *str);

// time.c
uintmax_t	retrieve_time_us(void);
uintmax_t	retrieve_time_since_ms(uintmax_t start);


int			fill_args(t_arg *args, char **argv);
void		valid_args(t_arg *args);
int			start_philo_threads(t_arg *args, t_philo *philos);
void	print_message(t_philo *philo, const char *message, t_msg_type type);
void	set_philo_mutexes(int idx, t_philo *philo, pthread_mutex_t *locks,
			pthread_mutex_t **p_locks);
void	*handle_full_philo(t_philo *philo);
int	set_dead(t_philo *philo);

#endif