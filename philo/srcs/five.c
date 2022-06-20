#include "../includes/philo.h"

size_t	ft_strlen(const char *str)
{
	size_t	i;

	i = 0;
	while (str[i] != '\0')
		i++;
	return (i);
}

uintmax_t	retrieve_time_us(void)
{
	struct timeval	time;
	uintmax_t		time_usec;

	gettimeofday(&time, NULL);
	time_usec = (time.tv_sec * 1000) * 1000;
	time_usec += time.tv_usec;
	return (time_usec);
}

int	lock_check(t_philo *philo, pthread_mutex_t *lock, const char *fn)
{
	if (pthread_mutex_lock(lock) != 0)
	{
		printf("philo [%d] | FAILED to lock in %s\n", philo->seat, fn);
		return (1);
	}
	return (0);
}

int	update_eat_time(t_philo *philo)
{
	if (lock_check(philo, &philo->eat_lock, "update_eat_time") != 0)
		return (1);
	philo->last_ate_ms = retrieve_time_since_ms(0);
	philo->deadline = philo->last_ate_ms + philo->arg->die_ms;
	pthread_mutex_unlock(&philo->eat_lock);
	return (0);
}

void	u_sleep_better(uintmax_t usec)
{
	uintmax_t	start;

	start = retrieve_time_us();
	while (retrieve_time_us() - start < usec)
		usleep(100);
}

int	clean_exit(t_arg *args, t_philo **philos)
{
	int	i;

	i = 0;
	if (args != NULL)
	{
		pthread_mutex_destroy(&args->msg_lock);
		pthread_mutex_destroy(&args->dead_lock);
	}
	if (philos != NULL && args->init.philos)
	{
		while (i < args->philos)
		{
			pthread_mutex_destroy(&((*philos)[i].eat_lock));
			pthread_mutex_destroy(((*philos)[i].p_forks[LEFT]));
			i++;
		}
		free(*philos);
	}
	if (args->init.fork)
		free(args->forks);
	exit(0);
}

void	print_message(t_philo *philo, const char *message, t_msg_type type)
{
	uintmax_t	time;

	time = retrieve_time_since_ms(philo->arg->start_ms);
	printf("%s", get_philo_colour(philo->seat));
	printf("%ju\tphilosopher [%d] %s.", time, philo->seat, message);
	if (philo->arg->no_of_eat && type == EAT)
		printf(" (%d/%d)", philo->times_eaten + 1, philo->arg->no_of_eat);
	printf("\n%s", WHT);
}

int	set_dead(t_philo *philo)
{
	if (lock_check(philo, &philo->arg->dead_lock, "set_dead") != 0)
		return (1);
	philo->arg->is_dead = true;
	pthread_mutex_unlock(&philo->arg->dead_lock);
	return (0);
}

