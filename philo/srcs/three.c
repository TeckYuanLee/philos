#include "../includes/philo.h"

int	philo_eat(t_philo *philo)
{
	if (update_eat_time(philo))
		return (1);
	status_change_message(philo, MSG_EAT, EAT);
	u_sleep_better(philo->arg->eat_ms * 1000);
	philo->times_eaten++;
	philo_clean_forks(philo);
	return (0);
}

int	philo_take_forks(t_philo *philo)
{
	int	side;

	if (philo->seat % 2 == 0)
		side = LEFT;
	else
		side = RIGHT;
	if (lock_check(philo, philo->p_forks[side], "philo_take_forks") != 0)
		return (1);
	status_change_message(philo, MSG_FORK, FORK);
	if (lock_check(philo, philo->p_forks[!side], "philo_take_forks") != 0)
	{
		pthread_mutex_unlock(philo->p_forks[side]);
		return (1);
	}
	status_change_message(philo, MSG_FORK, FORK);
	return (0);
}

bool	has_eaten_enough(t_philo *philo)
{
	bool	enough;

	enough = false;
	if (lock_check(philo, &philo->eat_lock, "has_eaten_enough") != 0)
		return (1);
	if (philo->arg->no_of_eat > 0
		&& philo->times_eaten == philo->arg->no_of_eat)
		enough = true;
	pthread_mutex_unlock(&philo->eat_lock);
	return (enough);
}

bool	gameover(t_philo *philo)
{
	bool	done;

	done = false;
	if (lock_check(philo, &philo->arg->dead_lock, "gameover") != 0)
		return (1);
	if (philo->arg->is_dead == true
		|| philo->arg->full_philos == philo->arg->philos)
		done = true;
	pthread_mutex_unlock(&philo->arg->dead_lock);
	return (done);
}

void	*handle_full_philo(t_philo *philo)
{
	status_change_message(philo, MSG_ENOUGH, ENOUGH);
	philo->arg->full_philos++;
	return (NULL);
}