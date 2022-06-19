#include "../includes/philo.h"

uintmax_t	retrieve_time_us(void)
{
	struct timeval	time;
	uintmax_t		time_usec;

	gettimeofday(&time, NULL);
	time_usec = (time.tv_sec * 1000) * 1000;
	time_usec += time.tv_usec;
	return (time_usec);
}

int	lock_check(t_philo *philo, pthread_mutex_t *lock, const char *fn)
{
	if (pthread_mutex_lock(lock) != 0)
	{
		printf("philo [%d] | FAILED to lock in %s\n", philo->seat, fn);
		return (1);
	}
	return (0);
}

int	update_eat_time(t_philo *philo)
{
	if (lock_check(philo, &philo->eat_lock, "update_eat_time") != 0)
		return (1);
	philo->last_ate_msec = retrieve_time_since_ms(0);
	philo->deadline = philo->last_ate_msec + philo->setup->msec_to_die;
	pthread_mutex_unlock(&philo->eat_lock);
	return (0);
}

void	u_sleep_better(uintmax_t usec)
{
	uintmax_t	start;

	start = retrieve_time_us();
	while (retrieve_time_us() - start < usec)
		usleep(100);
}

int	print_error(const char *message)
{
	if (message)
		write(STDERR_FILENO, message, ft_strlen(message));
	return (1);
}

int	clean_exit(t_arg *args, t_philo **philos)
{
	int	i;

	i = 0;
	if (args != NULL)
	{
		pthread_mutex_destroy(&args->msg_lock);
		pthread_mutex_destroy(&args->dead_lock);
	}
	if (philos != NULL && args->init.philos_init)
	{
		while (i < args->philos)
		{
			pthread_mutex_destroy(&((*philos)[i].eat_lock));
			pthread_mutex_destroy(((*philos)[i].p_forks[LEFT]));
			i++;
		}
		free(*philos);
	}
	if (args->init.fork_init)
		free(args->forks);
	return (0);
}

const char	*get_philo_colour(int philo_seat)
{
	static const char	*colour[] = {RED, GRN, YEL, BLU, MAG, CYN};

	return (colour[philo_seat % 6]);
}

void	philo_clean_forks(t_philo *philo)
{
	pthread_mutex_unlock(philo->p_forks[LEFT]);
	pthread_mutex_unlock(philo->p_forks[RIGHT]);
	return ;
}

int	status_change_message(t_philo *philo, const char *message, t_msg_type type)
{
	if (lock_check(philo, &philo->arg->msg_lock,
			"status_change_message") != 0)
		return (1);
	if (!gameover(philo))
	{
		print_message(philo, message, type);
		if (type == DEAD)
			set_dead(philo);
	}
	pthread_mutex_unlock(&philo->arg->msg_lock);
	return (0);
}

int	philo_think(t_philo *philo)
{
	if (status_change_message(philo, MSG_THINK, THINK))
		return (1);
	return (0);
}

int	philo_sleep(t_philo *philo)
{
	if (status_change_message(philo, MSG_SLEEP, SLEEP))
		return (1);
	u_sleep_better(philo->arg->sleep_ms * 1000);
	return (0);
}

int	philo_eat(t_philo *philo)
{
	if (update_eat_time(philo))
		return (1);
	status_change_message(philo, MSG_EAT, EAT);
	u_sleep_better(philo->arg->eat_ms * 1000);
	philo->times_eaten++;
	philo_clean_forks(philo);
	return (0);
}

int	philo_take_forks(t_philo *philo)
{
	int	side;

	if (philo->seat % 2 == 0)
		side = LEFT;
	else
		side = RIGHT;
	if (lock_check(philo, philo->p_forks[side], "philo_take_forks") != 0)
		return (1);
	status_change_message(philo, MSG_FORK, FORK);
	if (lock_check(philo, philo->p_forks[!side], "philo_take_forks") != 0)
	{
		pthread_mutex_unlock(philo->p_forks[side]);
		return (1);
	}
	status_change_message(philo, MSG_FORK, FORK);
	return (0);
}

bool	has_eaten_enough(t_philo *philo)
{
	bool	enough;

	enough = false;
	if (lock_check(philo, &philo->eat_lock, "has_eaten_enough") != 0)
		return (1);
	if (philo->arg->must_eat > 0
		&& philo->times_eaten == philo->arg->must_eat)
		enough = true;
	pthread_mutex_unlock(&philo->eat_lock);
	return (enough);
}

bool	gameover(t_philo *philo)
{
	bool	done;

	done = false;
	if (lock_check(philo, &philo->arg->dead_lock, "gameover") != 0)
		return (1);
	if (philo->arg->is_dead == true
		|| philo->arg->full_philos == philo->arg->philos)
		done = true;
	pthread_mutex_unlock(&philo->arg->dead_lock);
	return (done);
}

void	*handle_full_philo(t_philo *philo)
{
	status_change_message(philo, MSG_ENOUGH, ENOUGH);
	philo->arg->full_philos++;
	return (NULL);
}

void	*handle_death(t_philo *philo)
{
	status_change_message(philo, MSG_DIED, DEAD);
	pthread_mutex_unlock(&philo->eat_lock);
	philo_clean_forks(philo);
	return (NULL);
}

void	*philo_check(void *philo_arg)
{
	t_philo		*philo;
	uintmax_t	current_time;

	philo = (t_philo *)philo_arg;
	while (1)
	{
		if (!philo->active)
			break ;
		if (lock_check(philo, &philo->eat_lock, "philo_check") != 0)
			return ((void *)1);
		current_time = retrieve_time_since_ms(0);
		if (current_time > philo->deadline)
			return (handle_death(philo));
		pthread_mutex_unlock(&philo->eat_lock);
		if (philo->arg->must_eat > 0 && has_eaten_enough(philo))
			return (handle_full_philo(philo));
		usleep(1000);
	}
	return (NULL);
}

void	*philo_do(void *philo_arg)
{
	t_philo		*philo;
	pthread_t	tid;

	philo = (t_philo *)philo_arg;
	philo->active = true;
	if (pthread_create(&tid, NULL, &philo_check, philo_arg) != 0)
		return ((void *)1);
	if (philo->seat % 2 == 0)
		usleep(1000);
	if (update_eat_time(philo) == 0)
	{
		while (!gameover(philo) && !has_eaten_enough(philo))
		{
			if (philo_take_forks(philo)
				|| philo_eat(philo)
				|| philo_sleep(philo)
				|| philo_think(philo))
				break ;
			usleep(1000);
		}
	}
	philo->active = false;
	pthread_join(tid, NULL);
	return (NULL);
}

int	start_philo_threads(t_arg *args, t_philo *philos)
{
	int			i;

	i = -1;
	while (philos && ++i < args->philos)
	{
		if (pthread_create(&philos[i].id, NULL, &philo_do,
				&philos[i]) != 0)
			return (1);
	}
	args->start_ms = retrieve_time_since_ms(0);
	i = -1;
	while (++i < args->philos)
		pthread_join(philos[i].id, NULL);
	return (0);
}

void	set_philo_mutexes(int idx, t_philo *philo, pthread_mutex_t *locks,
			pthread_mutex_t **p_locks)
{
	p_locks[LEFT] = &locks[idx];
	if (idx == philo->arg->philos - 1)
		p_locks[RIGHT] = &locks[0];
	else
		p_locks[RIGHT] = &locks[idx + 1];
	return ;
}

int	init_philos(t_arg *args, t_philo **philos)
{
	int				i;

	*philos = NULL;
	*philos = malloc(sizeof(t_philo) * args->philos);
	if (*philos == NULL)
		return (1);
	i = -1;
	while (++i < args->philos)
	{
		(*philos)[i].id = NULL;
		(*philos)[i].seat = i + 1;
		(*philos)[i].active = false;
		(*philos)[i].times_eaten = 0;
		(*philos)[i].last_ate_ms = retrieve_time_since_ms(0);
		(*philos)[i].deadline = (*philos)[i].last_ate_ms + args->die_ms;
		(*philos)[i].arg = args;
		if (pthread_mutex_init(&(*philos)[i].eat_lock, NULL) != 0)
			return (1);
		set_philo_mutexes(i, &(*philos)[i], args->forks, (*philos)[i].p_forks);
	}
	args->init.philos_init = 1;
	return (0);
}

int	init_forks(t_arg *args)
{
	int	i;

	args->forks = malloc(sizeof(pthread_mutex_t) * args->philos);
	if (args->forks == NULL)
		return (1);
	i = -1;
	while (++i < args->philos)
	{
		if (pthread_mutex_init(&(args->forks[i]), NULL) != 0)
			return (1);
	}
	args->init.fork_init = 1;
	return (0);
}

int	init_philos_and_mutexes(t_arg *args, t_philo **philos)
{
	if (init_forks(args))
		return (1);
	if (init_philos(args, philos))
		return (1);
	return (0);
}

bool	valid_args(t_arg *args)
{
	if (args->philos < 1 || args->philos > 200
		|| args->die_ms < 60
		|| args->eat_ms < 60
		|| args->sleep_ms < 60
		|| args->must_eat < 0)
		return (false);
	return (true);
}

int	fill_args(t_arg *args, int *converted_args)
{
	args->philos = converted_args[0];
	args->die_ms = converted_args[1];
	args->eat_ms = converted_args[2];
	args->sleep_ms = converted_args[3];
	args->must_eat = converted_args[4];
	return (0);
}

bool	is_whitespace(const char c)
{
	if (c == '\t' || c == '\n' || c == '\v'
		|| c == '\f' || c == '\r' || c == ' ')
		return (true);
	return (false);
}

int	ft_skipws(const char **str, int i)
{
	if (*str == NULL || *str[i] == '\0' || !is_whitespace(*str[i]))
		return (0);
	while (is_whitespace(*str[i]))
		(*str)++;
	return (1);
}

bool	valid_atoia(const char *num_str, int *num)
{
	bool		is_negative;
	intmax_t	res;

	res = 0;
	if (!num_str)
		return (false);
	ft_skipws(&num_str, 0);
	is_negative = (*num_str == '-');
	if (*num_str == '-' || *num_str == '+')
		num_str++;
	while (*num_str != '\0')
	{
		if (*num_str < '0' || *num_str > '9')
			return (false);
		res = res * 10 + (*num_str - '0');
		if ((!is_negative && res > INT_MAX)
			|| (is_negative && -(res) < INT_MIN))
			return (false);
		num_str++;
	}
	if (is_negative)
		*num = res * -1;
	else
		*num = res;
	return (true);
}

int	parse_args_into_setup(t_arg *args, const char **argv)
{
	int	i;
	int	converted_argv[5];

	i = 0;
	while (argv[i] != NULL)
	{
		converted_argv[i] = 0;
		if (!valid_atoia(argv[i], &converted_argv[i]))
			return (1);
		i++;
	}
	if (i == 4)
		converted_argv[4] = 0;
	if (fill_args(args, converted_argv))
		return (1);
	if (!valid_args(args))
		return (1);
	return (0);
}

uintmax_t	retrieve_time_since_ms(uintmax_t start)
{
	struct timeval	time_now;
	uintmax_t		time_ms;

	gettimeofday(&time_now, NULL);
	time_ms = (time_now.tv_sec * 1000) + (time_now.tv_usec / 1000);
	if (time_ms < start)
		return (0);
	return (time_ms - start);
}

void	init_setup(t_arg *args)
{
	args->philos = 0;
	args->die_ms = 0;
	args->eat_ms = 0;
	args->sleep_ms = 0;
	args->must_eat = 0;
	args->full_philos = 0;
	args->start_ms = retrieve_time_since_ms(0);
	args->is_dead = false;
	if (pthread_mutex_init(&args->msg_lock, NULL) != 0)
		return ;
	if (pthread_mutex_init(&args->dead_lock, NULL) != 0)
		return ;
	args->init.fork_init = false;
	args->init.philos_init = false;
	return ;
}

int	main(int argc, char **argv)
{
	t_arg	args;
	t_philo	*philos;

	if (argc > 4 && argc < 7)
	{
		init_setup(&args);
		if (parse_args_into_setup(&args, (const char **)(argv + 1)))
			return (print_error(ERR_SETUP) | clean_exit(&args, NULL));
		if (init_philos_and_mutexes(&args, &philos))
			return (print_error(ERR_GEN) | clean_exit(&args, &philos));
		if (start_philo_threads(&args, philos))
			return (print_error(ERR_GEN) | clean_exit(&args, &philos));
		clean_exit(&args, &philos);
	}
	return (0);
}