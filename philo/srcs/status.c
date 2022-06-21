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

int	set_dead(t_philo *philo)
{
	if (lock_check(philo, &philo->arg->dead_lock, "set_dead") != 0)
		return (1);
	philo->arg->is_dead = true;
	pthread_mutex_unlock(&philo->arg->dead_lock);
	return (0);
}

int	status_change_message(t_philo *philo, const char *message, t_msg_type type)
{
	if (lock_check(philo, &philo->arg->msg_lock, "status_change_message"))
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

void	*handle_full_philo(t_philo *philo)
{
	status_change_message(philo, MSG_ENOUGH, ENOUGH);
	philo->arg->full_philos++;
	return (NULL);
}

void	*handle_death(t_philo *philo)
{
	status_change_message(philo, MSG_DIED, DEAD);
	pthread_mutex_unlock(&philo->eat_lock);
	philo_clean_forks(philo);
	return (NULL);
}