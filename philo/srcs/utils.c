#include "../includes/philo.h"

void	u_sleep_better(uintmax_t usec)
{
	struct timeval	time;
	uintmax_t		start;
	uintmax_t		end;
	
	gettimeofday(&time, NULL);
	start = (time.tv_sec * 1000000) + time.tv_usec;
	while ((gettimeofday(&time, NULL)
			| (time.tv_sec * 1000000) + time.tv_usec - start) < usec)
		usleep(100);
}

uintmax_t	retrieve_time_since_ms(uintmax_t start)
{
	struct timeval	time_now;
	uintmax_t		time_ms;

	gettimeofday(&time_now, NULL);
	time_ms = (time_now.tv_sec * 1000) + (time_now.tv_usec / 1000);
	if (time_ms < start)
		return (0);
	return (time_ms - start);
}

int	update_eat_time(t_philo *philo)
{
	if (lock_check(philo, &philo->eat_lock, "update_eat_time"))
		return (1);
	philo->last_ate_ms = retrieve_time_since_ms(0);
	philo->deadline = philo->last_ate_ms + philo->arg->die_ms;
	pthread_mutex_unlock(&philo->eat_lock);
	return (0);
}

int	ft_atoi(const char *str)
{
	long	integer;
	long	sign;

	integer = 0;
	sign = 1;
	if (!str)
		return (-1);
	while (*str && (*str == 32 || (*str > 8 && *str < 14)))
		str++;
	if (*str == '-')
		sign *= -1;
	if (*str == '-' || *str == '+')
		str++;
	while (*str)
	{
		if (*str < '0' || *str > '9')
			return (-1);
		integer = integer * 10 + (*str - '0');
		if ((sign * integer > INT_MAX) || (sign * integer < INT_MIN))
			return (-1);
		str++;
	}
	return (sign * integer);
}
