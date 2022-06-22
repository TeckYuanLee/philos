#include "../includes/philo.h"

int	clean_exit(t_arg *args, t_philo **philos)
{
	int	i;

	i = -1;
	if (args != NULL)
	{
		pthread_mutex_destroy(&args->msg_lock);
		pthread_mutex_destroy(&args->dead_lock);
	}
	if (philos != NULL && args->init.philos)
	{
		while (++i < args->philos)
		{
			pthread_mutex_destroy(&((*philos)[i].eat_lock));
			pthread_mutex_destroy(((*philos)[i].hands[LEFT]));
		}
		free(*philos);
	}
	if (args->init.fork)
		free(args->forks);
	exit(0);
}

uintmax_t	get_time_ms(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return ((time.tv_sec * 1000) + (time.tv_usec / 1000));
}

int	fill_args(t_arg *args, char **argv)
{
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

void	init_args(t_arg *args)
{
	args->philos = 0;
	args->die_ms = 0;
	args->eat_ms = 0;
	args->sleep_ms = 0;
	args->eat_no = 0;
	args->philos_eaten = 0;
	args->start_ms = get_time_ms();
	args->is_dead = false;
	if (pthread_mutex_init(&args->msg_lock, NULL))
		return ;
	if (pthread_mutex_init(&args->dead_lock, NULL))
		return ;
	args->init.fork = false;
	args->init.philos = false;
}

int	main(int argc, char **argv)
{
	t_arg	args;
	t_philo	*philos;

	if (argc > 4 && argc < 7)
	{
		init_args(&args);
		fill_args(&args, argv + 1);
		if (init_forks(&args) || init_philos(&args, &philos))
			return (printf(ERR_GEN) | clean_exit(&args, &philos));
		if (start_threads(&args, philos))
			return (printf(ERR_GEN) | clean_exit(&args, &philos));
		clean_exit(&args, &philos);
	}
	else
		return (printf(ERR_INPUT));
	return (0);
}
