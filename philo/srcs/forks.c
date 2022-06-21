#include "../includes/philo.h"

int	lock_check(t_philo *philo, pthread_mutex_t *lock, const char *fn)
{
	if (pthread_mutex_lock(lock) != 0)
	{
		printf("philo [%d] | FAILED to lock in %s\n", philo->seat, fn);
		return (1);
	}
	return (0);
}

int	status_change_message(t_philo *philo, const char *message, t_msg_type type)
{
	if (lock_check(philo, &philo->arg->msg_lock, "status_change_message"))
		return (1);
	if (!philo_died(philo))
	{
		print_message(philo, message, type);
		if (type == DEAD)
		{
			if (lock_check(philo, &philo->arg->dead_lock, "set_dead"))
				return (1);
			philo->arg->is_dead = true;
			pthread_mutex_unlock(&philo->arg->dead_lock);
		}
	}
	pthread_mutex_unlock(&philo->arg->msg_lock);
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
