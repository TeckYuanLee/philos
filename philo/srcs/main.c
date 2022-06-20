#include "../includes/philo.h"

int	ft_strcmp(void *s1, void *s2)
{
	int		i;
	char	*str1;
	char	*str2;

	i = 0;
	str1 = s1;
	str2 = s2;
	while ((str1[i] && str2[i]) && str1[i] == str2[i])
		i++;
	if (!str1[i] && !str2[i])
		return (0);
	return (str1[i] - str2[i]);
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

int	ft_atoi(const char *str)
{
	long	integer;
	long	sign;

	integer = 0;
	sign = 1;
	if (!str)
		return (-1);
	while (*str && (*str == 32 || (*str > 8 && *str < 14)))
		str++;
	if (*str == '-')
		sign *= -1;
	if (*str == '-' || *str == '+')
		str++;
	while (*str)
	{
		if (*str < '0' || *str > '9')
			return (-1);
		integer = integer * 10 + (*str - '0');
		if ((sign * integer > INT_MAX) || (sign * integer < INT_MIN))
			return (-1);
		str++;
	}
	return (sign * integer);
}

void	valid_args(t_arg *args)
{
	if (args->philos < 1 || args->philos > 200
		|| args->die_ms < 60
		|| args->eat_ms < 60
		|| args->sleep_ms < 60
		|| args->no_of_eat < 0)
		printf(ERR_ARGS ERR_MS) | clean_exit(args, NULL);
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
		valid_args(&args);
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