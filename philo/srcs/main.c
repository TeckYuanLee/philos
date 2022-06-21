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
			pthread_mutex_destroy(((*philos)[i].p_forks[LEFT]));
		}
		free(*philos);
	}
	if (args->init.fork)
		free(args->forks);
	exit(0);
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

int	fill_args(t_arg *args, char **argv)
{
	args->philos = ft_atoi(argv[0]);
	args->die_ms = ft_atoi(argv[1]);
	args->eat_ms = ft_atoi(argv[2]);
	args->sleep_ms = ft_atoi(argv[3]);
	args->no_of_eat = ft_atoi(argv[4]);
    if (args->no_of_eat == 0)
        return (printf(ERR_ARGS ERR_MS) | clean_exit(args, NULL));
    else if (!(argv[4]))
        args->no_of_eat = 0;
	if (args->philos < 1 || args->philos > 200 || args->die_ms < 60
		|| args->eat_ms < 60 || args->sleep_ms < 60 || args->no_of_eat < 0)
		printf(ERR_ARGS ERR_MS) | clean_exit(args, NULL);
	return (0);
}

void	init_args(t_arg *args)
{
	args->philos = 0;
	args->die_ms = 0;
	args->eat_ms = 0;
	args->sleep_ms = 0;
	args->no_of_eat = 0;
	args->philos_eaten = 0;
	args->start_ms = retrieve_time_since_ms(0);
	args->is_dead = false;
	if (pthread_mutex_init(&args->msg_lock, NULL))
		return ;
	if (pthread_mutex_init(&args->dead_lock, NULL))
		return ;
	args->init.fork = false;
	args->init.philos = false;
	return ;
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
		if (start_philo_threads(&args, philos))
			return (printf(ERR_GEN) | clean_exit(&args, &philos));
		clean_exit(&args, &philos);
	}
	else
		return (printf(ERR_INPUT));
	return (0);
}
