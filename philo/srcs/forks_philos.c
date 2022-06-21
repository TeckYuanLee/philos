#include "../includes/philo.h"

void	*philo_start(void *philo_arg)
{
	t_philo		*philo;
	pthread_t	tid;

	philo = (t_philo *)philo_arg;
	philo->active = true;
	if (pthread_create(&tid, NULL, &philo_check, philo_arg))
		return ((void *)1);
	if (philo->seat % 2 == 0)
		usleep(1000);
	update_eat_time(philo);
	while (!philo_end(philo) && !done_eating(philo))
	{
		philo_action(philo);
		usleep(1000);
	}
	philo->active = false;
	pthread_join(tid, NULL);
	return (NULL);
}

int	start_philo_threads(t_arg *args, t_philo *philos)
{
	int	i;

	i = -1;
	while (philos && ++i < args->philos)
	{
		if (pthread_create(&philos[i].id, NULL, &philo_start, &philos[i]))
			return (1);
	}
	i = -1;
	while (++i < args->philos)
		pthread_join(philos[i].id, NULL); //makes sure all threads execute its task before quitting program
	return (0);
}

void	set_forks(int i, t_philo *philo, pthread_mutex_t *locks, pthread_mutex_t **p_locks)
{
	p_locks[LEFT] = &locks[i];
	if (i == philo->arg->philos - 1)
		p_locks[RIGHT] = &locks[0];
	else
		p_locks[RIGHT] = &locks[i + 1];
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
		set_forks(i, &(*philos)[i], args->forks, (*philos)[i].p_forks);
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
