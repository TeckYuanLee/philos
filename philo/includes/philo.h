#ifndef PHILO_H
# define PHILO_H

# include <limits.h>
# include <pthread.h>
# include <stdbool.h>
# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

# define ERR_INPUT "./philo [philos] [die_ms] [eat_ms] [sleep_ms] opt:[eat_no]\n"
# define ERR_ARGS "Error: Please provide valid arguments\n"
# define ERR_MS "=> Philos 1-200; die, eat, sleep >= 60; eat_no > 0\n"
# define ERR_GEN "Error initializing mutexes\n"

# define MSG_FORK " has taken a fork"
# define MSG_EAT " is eating"
# define MSG_SLEEP " is sleeping"
# define MSG_THINK " is thinking"
# define MSG_DIED " died"
# define MSG_EATEN " is done eating"

# define LEFT 0
# define RIGHT 1

# define BHRED "\e[1;91m"
# define BHGRN "\e[1;92m"
# define BHYEL "\e[1;93m"
# define BHBLU "\e[1;94m"
# define BHMAG "\e[1;95m"
# define BHCYN "\e[1;96m"
# define BHWHT "\e[1;97m"

typedef enum e_state {
	FORK = 0,
	EAT,
	SLEEP,
	THINK,
	DEAD,
	EATEN
}			t_state;

typedef struct s_init
{
	bool		fork;
	bool		philos;
}				t_init;

typedef struct s_arg
{
	int				philos;
	int				die_ms;
	int				eat_ms;
	int				sleep_ms;
	int				eat_no;
	int				philos_eaten;
	uintmax_t		start_ms;
	bool			is_dead;
	pthread_mutex_t	msg_lock;
	pthread_mutex_t	dead_lock;
	pthread_mutex_t	*forks;
	struct s_init	init;
}					t_arg;

typedef struct s_philo
{
	pthread_t		id;
	int				seat;
	bool			active;
	int				times_eaten;
	uintmax_t		eaten_ms;
	uintmax_t		deadline;
	struct s_arg	*arg;
	pthread_mutex_t	eat_lock;
	pthread_mutex_t	*hands[2];
}					t_philo;

void		init_args(t_arg *args);
int			fill_args(t_arg *args, char **argv);
uintmax_t	get_time_ms(void);
int			clean_exit(t_arg *args, t_philo **philos);

int			init_philos(t_arg *args, t_philo **philos);
int			init_forks(t_arg *args);
void		assign_forks(int i, t_philo *philo, t_arg *args);
int			start_threads(t_arg *args, t_philo *philos);
void		*philo_start(void *philo_arg);

void		*philo_check(void *philo_arg);
bool		philo_end(t_philo *philo);
bool		done_eating(t_philo *philo);
void		philo_action(t_philo *philo);
void		*philos_eaten_dead(t_philo *philo, char c);

void		update_state(t_philo *philo, const char *message, t_state state);
void		update_eaten_ms(t_philo *philo);
void		usleep_chunks(uintmax_t usec);
int			ft_atoi(const char *str);

#endif