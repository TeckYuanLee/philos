#include "../includes/philo.h"

void	u_sleep_better(uintmax_t usec)
{
	uintmax_t	start;

	start = retrieve_time_us();
	while (retrieve_time_us() - start < usec)
		usleep(100);
}

int	philo_think(t_philo *philo)
{
	if (status_change_message(philo, MSG_THINK, THINK))
		return (1);
	return (0);
}

int	philo_sleep(t_philo *philo)
{
	if (status_change_message(philo, MSG_SLEEP, SLEEP))
		return (1);
	u_sleep_better(philo->arg->sleep_ms * 1000);
	return (0);
}

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
