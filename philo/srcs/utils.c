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

void	update_eat_time(t_philo *philo)
{
	pthread_mutex_lock(&philo->eat_lock);
	philo->last_ate_ms = retrieve_time_since_ms(0);
	philo->deadline = philo->last_ate_ms + philo->arg->die_ms;
	pthread_mutex_unlock(&philo->eat_lock);
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

void	print_message(t_philo *philo, const char *message, t_msg_type type)
{
	uintmax_t	time;
	static const char	*colour[] = {RED, GRN, YEL, BLU, MAG, CYN};

	time = retrieve_time_since_ms(philo->arg->start_ms);
	printf("%s", colour[philo->seat % 6]);
	printf("%ju\tphilosopher [%d] %s.", time, philo->seat, message);
	if (philo->arg->no_of_eat && type == EAT)
		printf(" (%d/%d)", philo->times_eaten + 1, philo->arg->no_of_eat);
	printf("\n%s", WHT);
}

void	status_change_message(t_philo *philo, const char *message, t_msg_type type)
{
	pthread_mutex_lock(&philo->arg->msg_lock);
	if (!philo_end(philo))
	{
		print_message(philo, message, type);
		if (type == DEAD)
		{
			pthread_mutex_lock(&philo->arg->dead_lock);
			philo->arg->is_dead = true;
			pthread_mutex_unlock(&philo->arg->dead_lock);
		}
	}
	pthread_mutex_unlock(&philo->arg->msg_lock);
}
