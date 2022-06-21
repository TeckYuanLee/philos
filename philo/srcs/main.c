#include "../includes/philo.h"

void	print_message(t_philo *philo, const char *message, t_msg_type type)
{
	uintmax_t	time;
	static const char	*colour[] = {RED, GRN, YEL, BLU, MAG, CYN};

	time = retrieve_time_since_ms(philo->arg->start_ms);
	printf("%s", colour[philo->seat % 6]);
	printf("%ju\tphilosopher [%d] %s.", time, philo->seat, message);
	if (philo->arg->no_of_eat && type == EAT)
		printf(" (%d/%d)", philo->times_eaten + 1, philo->arg->no_of_eat);
	printf("\n%s", WHT);
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
    // printf("%d %d %d %d %d\n", args->philos, args->die_ms, args->eat_ms, args->sleep_ms, args->no_of_eat);
	return (0);
}

void	init_args(t_arg *args)
{
	args->philos = 0;
	args->die_ms = 0;
	args->eat_ms = 0;
	args->sleep_ms = 0;
	args->no_of_eat = 0;
	args->full_philos = 0;
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