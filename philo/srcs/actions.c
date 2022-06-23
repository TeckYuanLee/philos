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

void	*philos_eaten_dead(t_philo *philo, t_state state)
{
	if (state == EATEN)
	{
		update_state(philo, MSG_EATEN, EATEN);
		philo->arg->philos_eaten++;
	}
	if (state == DEAD)
	{
		update_state(philo, MSG_DIED, DEAD);
		pthread_mutex_lock(&philo->arg->lock.dead);
		philo->arg->is_dead = true;
		pthread_mutex_unlock(&philo->arg->lock.dead);
		pthread_mutex_unlock(&philo->arg->lock.eat);
		pthread_mutex_unlock(philo->hands[LEFT]);
		pthread_mutex_unlock(philo->hands[RIGHT]);
	}
	return (NULL);
}

// bool	done_eating(t_philo *philo)
// {
// 	bool	done;

// 	done = false;
// 	pthread_mutex_lock(&philo->arg->lock.eat);
// 	if (philo->arg->eat_no
// 		&& philo->times_eaten == philo->arg->eat_no)
// 		done = true;
// 	pthread_mutex_unlock(&philo->arg->lock.eat);
// 	return (done);
// }

// bool	philo_end(t_philo *philo)
// {
// 	bool	done;

// 	done = false;
// 	pthread_mutex_lock(&philo->arg->lock.dead);
// 	if (philo->arg->is_dead
// 		|| philo->arg->philos_eaten == philo->arg->philos)
// 		done = true;
// 	pthread_mutex_unlock(&philo->arg->lock.dead);
// 	return (done);
// }

bool	philo_status(t_philo *philo, t_state state)
{
	bool	done;

	if (state == EATEN || state == END)
	{
		done = false;
		pthread_mutex_lock(&philo->arg->lock.eat);
		if (philo->arg->eat_no
			&& philo->times_eaten == philo->arg->eat_no)
			done = true;
		pthread_mutex_unlock(&philo->arg->lock.eat);
		if (state == EATEN)
			return (done);
	}
	if (state == DEAD || (state == END && done == false))
	{
		done = false;
		pthread_mutex_lock(&philo->arg->lock.dead);
		if (philo->arg->is_dead
			|| philo->arg->philos_eaten == philo->arg->philos)
			done = true;
		pthread_mutex_unlock(&philo->arg->lock.dead);
		// return (done);
	}
	return (done);
}

void	*philo_check(void *philo_arg)
{
	t_philo		*philo;

	philo = (t_philo *)philo_arg;
	while (philo->active)
	{
		pthread_mutex_lock(&philo->arg->lock.eat);
		if (get_time_ms() > philo->deadline)
			return (philos_eaten_dead(philo, DEAD));
		pthread_mutex_unlock(&philo->arg->lock.eat);
		// if (philo->arg->eat_no && done_eating(philo))
		if (philo->arg->eat_no && philo_status(philo, EATEN))
			return (philos_eaten_dead(philo, EATEN));
		usleep(1000);
	}
	return (NULL);
}
