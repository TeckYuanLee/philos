#include "../includes/philo.h"

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
		if (philo->arg->no_of_eat > 0 && has_eaten_enough(philo))
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