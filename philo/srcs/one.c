#include "../includes/philo.h"

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
		(*philos)[i].id = 0;
		(*philos)[i].seat = i + 1;
		(*philos)[i].active = false;
		(*philos)[i].times_eaten = 0;
		(*philos)[i].last_ate_ms = retrieve_time_since_ms(0);
		(*philos)[i].deadline = (*philos)[i].last_ate_ms + args->die_ms;
		(*philos)[i].arg = args;
		if (pthread_mutex_init(&(*philos)[i].eat_lock, NULL))
			return (1);
		set_philo_mutexes(i, &(*philos)[i], args->forks, (*philos)[i].p_forks);
	}
	args->init.philos = true;
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
		if (pthread_mutex_init(&(args->forks[i]), NULL))
			return (1);
	}
	args->init.fork = true;
	return (0);
}
