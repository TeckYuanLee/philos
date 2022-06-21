#include "../includes/philo.h"

int	philo_eat_sleep_think(t_philo *philo)
{
	if (update_eat_time(philo))
		return (1);
	status_change_message(philo, MSG_EAT, EAT);
	u_sleep_better(philo->arg->eat_ms * 1000);
	philo->times_eaten++;
	pthread_mutex_unlock(philo->p_forks[LEFT]);
	pthread_mutex_unlock(philo->p_forks[RIGHT]);
	if (status_change_message(philo, MSG_SLEEP, SLEEP))
		return (1);
	u_sleep_better(philo->arg->sleep_ms * 1000);
	if (status_change_message(philo, MSG_THINK, THINK))
		return (1);
	return (0);
}

int	philo_take_forks(t_philo *philo)
{
	int	side;

	if (philo->seat % 2 == 0)
		side = LEFT;
	else
		side = RIGHT;
	if (lock_check(philo, philo->p_forks[side], "philo_take_forks"))
		return (1);
	status_change_message(philo, MSG_FORK, FORK);
	if (lock_check(philo, philo->p_forks[!side], "philo_take_forks"))
	{
		pthread_mutex_unlock(philo->p_forks[side]);
		return (1);
	}
	status_change_message(philo, MSG_FORK, FORK);
	return (0);
}

void	*handle_eaten_death(t_philo *philo, char c)
{
	if (c == 'e')
	{
		status_change_message(philo, MSG_ENOUGH, ENOUGH);
		philo->arg->full_philos++;
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
