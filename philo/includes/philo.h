#ifndef PHILO_H
# define PHILO_H

# include <stdio.h>
# include <stdlib.h>

# include <unistd.h>
# include <string.h>
# include <pthread.h>
# include <sys/time.h>
# include <stdint.h>

# define RED "\e[0;31m"
# define GRN "\e[0;32m"
# define YEL "\e[0;33m"
# define BLU "\e[0;34m"
# define MAG "\e[0;35m"
# define CYN "\e[0;36m"
# define WHT "\e[0;37m"

typedef struct s_init
{
	bool		fork_init;
	bool		philos_init;
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
	int				must_eat;
	int				full_philos;
	uintmax_t		start_ms;
	bool			is_dead;
	pthread_mutex_t	msg_lock;
	pthread_mutex_t	dead_lock;
	pthread_mutex_t	*forks;
	struct s_init	init;
}					t_arg;

void	taking_forks(t_philo *philo);
void	eating(t_philo *philo);
void	sleeping(t_philo *philo);
void	thinking(t_philo *philo);
long	ft_time(void);
void	ft_usleep(int ms);
int		ft_atoi(const char *str);
int		ft_is_digit(char *str);
int		ft_cnt_of_meals(t_philo *philo);
void	*ft_galina_monitor(void *args);
void	*ft_process(void *args);
void	ft_init_philosophers(t_arg *args);
void	ft_init_mutex(t_arg *args);
void	ft_init_threads(t_arg *args);
void	ft_end_threads(t_arg *args);
int		ft_init_args(t_arg *args, int argc, char **argv);
void	ft_check_args(void);
void	free_all(t_arg *args);
void	unlock_and_destroy_mutex(t_arg *args);

// check.c
void		*philo_check(void *philo_arg);

// exit.c
int			print_error(const char *message);
int			clean_exit(t_setup *setup, t_philo **philos);

// fork.c
void		philo_clean_forks(t_philo *philo);
int			philo_take_forks(t_philo *philo);

// init.c
int			init_philos(t_setup *setup, t_philo **philos);
int			init_forks(t_setup *setup);
int			init_philos_and_mutexes(t_setup *setup, t_philo **philos);
void		init_setup(t_setup *setup);

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

// prog_utils.c
const char	*get_philo_colour(int philo_seat);
int			ft_strcmp(void *s1, void *s2);
bool		is_whitespace(const char c);
int			ft_skipws(const char **str, int i);
bool		valid_atoia(const char *num_str, int *num);

// setup.c
int			parse_args_into_setup(t_setup *setup, const char **args);

// time.c
uintmax_t	retrieve_time_us(void);
uintmax_t	retrieve_time_since_ms(uintmax_t start);

#endif