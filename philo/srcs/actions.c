#include "../includes/philo.h"

void	philo_action(t_philo *philo)
{
	int	side;

	if (philo->seat % 2 == 0)
		side = LEFT;
	else
		side = RIGHT;
	pthread_mutex_lock(philo->p_forks[side]);
	status_change_message(philo, MSG_FORK, FORK);
	pthread_mutex_lock(philo->p_forks[!side]);
	status_change_message(philo, MSG_FORK, FORK);
	update_eat_time(philo);
	status_change_message(philo, MSG_EAT, EAT);
	u_sleep_better(philo->arg->eat_ms * 1000);
	philo->times_eaten++;
	pthread_mutex_unlock(philo->p_forks[LEFT]);
	pthread_mutex_unlock(philo->p_forks[RIGHT]);
	status_change_message(philo, MSG_SLEEP, SLEEP);
	u_sleep_better(philo->arg->sleep_ms * 1000);
	status_change_message(philo, MSG_THINK, THINK);
}

void	*handle_eaten_death(t_philo *philo, char c)
{
	if (c == 'e')
	{
		status_change_message(philo, MSG_ENOUGH, ENOUGH);
		philo->arg->philos_eaten++;
	}
	if (c == 'd')
	{
		status_change_message(philo, MSG_DIED, DEAD);
		pthread_mutex_unlock(&philo->eat_lock);
		pthread_mutex_unlock(philo->p_forks[LEFT]);
		pthread_mutex_unlock(philo->p_forks[RIGHT]);
	}
	return (NULL);
}

bool	done_eating(t_philo *philo)
{
	bool	done;

	done = false;
	pthread_mutex_lock(&philo->eat_lock);
	if (philo->arg->no_of_eat > 0
		&& philo->times_eaten == philo->arg->no_of_eat)
		done = true;
	pthread_mutex_unlock(&philo->eat_lock);
	return (done);
}

bool	philo_end(t_philo *philo)
{
	bool	done;

	done = false;
	pthread_mutex_lock(&philo->arg->dead_lock);
	if (philo->arg->is_dead == true
		|| philo->arg->philos_eaten == philo->arg->philos)
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
		pthread_mutex_lock(&philo->eat_lock);
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
