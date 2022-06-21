#include "../includes/philo.h"

void	philo_clean_forks(t_philo *philo)
{
	pthread_mutex_unlock(philo->p_forks[LEFT]);
	pthread_mutex_unlock(philo->p_forks[RIGHT]);
	return ;
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
	if (philos != NULL && args->init.philos)
	{
		while (i < args->philos)
		{
			pthread_mutex_destroy(&((*philos)[i].eat_lock));
			pthread_mutex_destroy(((*philos)[i].p_forks[LEFT]));
			i++;
		}
		free(*philos);
	}
	if (args->init.fork)
		free(args->forks);
	exit(0);
}

void	set_philo_mutexes(int i, t_philo *philo, pthread_mutex_t *locks,
			pthread_mutex_t **p_locks)
{
	p_locks[LEFT] = &locks[i];
	if (i == philo->arg->philos - 1)
		p_locks[RIGHT] = &locks[0];
	else
		p_locks[RIGHT] = &locks[i + 1];
	// printf("%s lol %s lol\n", (char *)p_locks[LEFT], (char *)p_locks[RIGHT]);
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