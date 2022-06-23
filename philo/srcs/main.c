#include "../includes/philo.h"

int	clean_exit(t_arg *args, t_philo **philos)
{
	int	i;

	i = -1;
	if (args != NULL)
	{
		pthread_mutex_destroy(&args->lock.msg);
		pthread_mutex_destroy(&args->lock.dead);
		pthread_mutex_destroy(&args->lock.eat);
	}
	if (philos != NULL && args->init.philos)
	{
		while (++i < args->philos)
			pthread_mutex_destroy(((*philos)[i].hands[LEFT]));
		free(*philos);
	}
	if (args->init.fork)
		free(args->lock.forks);
	exit(0);
}

void	update_state(t_philo *philo, char *message, t_state state)
{
	uintmax_t		time;
	static char		*colour[] = {BHRED, BHGRN, BHCYN, BHBLU, BHMAG, BHYEL};

	pthread_mutex_lock(&philo->arg->lock.msg);
	// if (!philo_end(philo))
	if (!philo_status(philo, DEAD))
	{
		time = get_time_ms();// - philo->arg->start_ms;
		printf("%s", colour[philo->seat % 6]);
		printf("%ju %d%s", time, philo->seat, message);
		if (philo->arg->eat_no && state == EAT)
			printf(" (%d/%d)", philo->times_eaten + 1, philo->arg->eat_no);
		printf("\n%s", BHWHT);
	}
	pthread_mutex_unlock(&philo->arg->lock.msg);
}

uintmax_t	get_time_ms(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return ((time.tv_sec * 1000) + (time.tv_usec / 1000));
}

int	init_args(t_arg *args, char **argv)
{
	args->philos = 0;
	args->die_ms = 0;
	args->eat_ms = 0;
	args->sleep_ms = 0;
	args->start_ms = get_time_ms();
	args->eat_no = 0;
	args->philos_eaten = 0;
	args->is_dead = false;
	args->init.fork = false;
	args->init.philos = false;
	args->philos = ft_atoi(argv[0]);
	args->die_ms = ft_atoi(argv[1]);
	args->eat_ms = ft_atoi(argv[2]);
	args->sleep_ms = ft_atoi(argv[3]);
	args->eat_no = ft_atoi(argv[4]);
	if (args->eat_no == 0)
		return (printf(ERR_ARGS ERR_MS) | clean_exit(args, NULL));
	else if (!(argv[4]))
		args->eat_no = 0;
	if (args->philos < 1 || args->philos > 200 || args->die_ms < 60
		|| args->eat_ms < 60 || args->sleep_ms < 60 || args->eat_no < 0)
		return (printf(ERR_ARGS ERR_MS) | clean_exit(args, NULL));
}

int	main(int argc, char **argv)
{
	t_arg	args;
	t_philo	*philos;

	if (argc > 4 && argc < 7)
	{
		init_args(&args, argv + 1);
		if (init_locks(&args) || init_philos(&args, &philos))
			return (printf(ERR_GEN) | clean_exit(&args, &philos));
		if (start_threads(&args, philos))
			return (printf(ERR_GEN) | clean_exit(&args, &philos));
		clean_exit(&args, &philos);
	}
	else
		return (printf(ERR_INPUT));
	return (0);
}
