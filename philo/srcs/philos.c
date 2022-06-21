#include "../includes/philo.h"

bool	done_eating(t_philo *philo)
{
	bool	done;

	done = false;
	if (lock_check(philo, &philo->eat_lock, "done_eating"))
		return (1);
	if (philo->arg->no_of_eat > 0
		&& philo->times_eaten == philo->arg->no_of_eat)
		done = true;
	pthread_mutex_unlock(&philo->eat_lock);
	return (done);
}

bool	philo_died(t_philo *philo)
{
	bool	done;

	done = false;
	if (lock_check(philo, &philo->arg->dead_lock, "philo_died"))
		return (1);
	if (philo->arg->is_dead == true
		|| philo->arg->full_philos == philo->arg->philos)
		done = true;
	pthread_mutex_unlock(&philo->arg->dead_lock);
	return (done);
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
			return (handle_eaten_death(philo, 'd'));
		pthread_mutex_unlock(&philo->eat_lock);
		if (philo->arg->no_of_eat > 0 && done_eating(philo))
			return (handle_eaten_death(philo, 'e'));
		usleep(1000);
	}
	return (NULL);
}

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
	if (update_eat_time(philo) == 0)
	{
		while (!philo_died(philo) && !done_eating(philo))
		{
			if (philo_take_forks(philo) || philo_eat_sleep_think(philo))
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
