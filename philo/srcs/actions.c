#include "../includes/philo.h"

void	philo_action(t_philo *philo)
{
	int	side;

	if (philo->seat % 2 == 0)
		side = LEFT;
	else
		side = RIGHT;
	pthread_mutex_lock(philo->hands[side]);
	update_state(philo, MSG_FORK, FORK);
	pthread_mutex_lock(philo->hands[!side]);
	update_state(philo, MSG_FORK, FORK);
	update_eaten_ms(philo);
	update_state(philo, MSG_EAT, EAT);
	usleep_chunks(philo->arg->eat_ms);
	philo->times_eaten++;
	pthread_mutex_unlock(philo->hands[LEFT]);
	pthread_mutex_unlock(philo->hands[RIGHT]);
	update_state(philo, MSG_SLEEP, SLEEP);
	usleep_chunks(philo->arg->sleep_ms);
	update_state(philo, MSG_THINK, THINK);
}

void	*philos_eaten_dead(t_philo *philo, char c)
{
	if (c == 'e')
	{
		update_state(philo, MSG_EATEN, EATEN);
		philo->arg->philos_eaten++;
	}
	if (c == 'd')
	{
		update_state(philo, MSG_DIED, DEAD);
		pthread_mutex_lock(&philo->arg->dead_lock);
		philo->arg->is_dead = true;
		pthread_mutex_unlock(&philo->arg->dead_lock);
		pthread_mutex_unlock(&philo->eat_lock);
		pthread_mutex_unlock(philo->hands[LEFT]);
		pthread_mutex_unlock(philo->hands[RIGHT]);
	}
	return (NULL);
}

bool	done_eating(t_philo *philo)
{
	bool	done;

	done = false;
	pthread_mutex_lock(&philo->eat_lock);
	if (philo->arg->eat_no
		&& philo->times_eaten == philo->arg->eat_no)
		done = true;
	pthread_mutex_unlock(&philo->eat_lock);
	return (done);
}

bool	philo_end(t_philo *philo)
{
	bool	done;

	done = false;
	pthread_mutex_lock(&philo->arg->dead_lock);
	if (philo->arg->is_dead
		|| philo->arg->philos_eaten == philo->arg->philos)
		done = true;
	pthread_mutex_unlock(&philo->arg->dead_lock);
	return (done);
}

void	*philo_check(void *philo_arg)
{
	t_philo		*philo;

	philo = (t_philo *)philo_arg;
	while (philo->active)
	{
		pthread_mutex_lock(&philo->eat_lock);
		if (get_time_ms() > philo->deadline)
			return (philos_eaten_dead(philo, 'd'));
		pthread_mutex_unlock(&philo->eat_lock);
		if (philo->arg->eat_no && done_eating(philo))
			return (philos_eaten_dead(philo, 'e'));
		usleep(1000);
	}
	return (NULL);
}
