#include "../includes/philo.h"

const char	*get_philo_colour(int philo_seat)
{
	static const char	*colour[] = {RED, GRN, YEL, BLU, MAG, CYN};

	return (colour[philo_seat % 6]);
}

void	philo_clean_forks(t_philo *philo)
{
	pthread_mutex_unlock(philo->p_forks[LEFT]);
	pthread_mutex_unlock(philo->p_forks[RIGHT]);
	return ;
}

int	status_change_message(t_philo *philo, const char *message, t_msg_type type)
{
	if (lock_check(philo, &philo->arg->msg_lock,
			"status_change_message") != 0)
		return (1);
	if (!gameover(philo))
	{
		print_message(philo, message, type);
		if (type == DEAD)
			set_dead(philo);
	}
	pthread_mutex_unlock(&philo->arg->msg_lock);
	return (0);
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