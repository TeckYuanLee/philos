#include "../includes/philo.h"

void	usleep_chunks(uintmax_t ms)
{
	struct timeval	time;
	uintmax_t		start;

	start = get_time_ms();
	while ((get_time_ms() - start) < ms)
		usleep(100);
}

void	update_eaten_ms(t_philo *philo)
{
	pthread_mutex_lock(&philo->eat_lock);
	philo->eaten_ms = get_time_ms();
	philo->deadline = philo->eaten_ms + philo->arg->die_ms;
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

void	update_state(t_philo *philo, const char *message, t_state state)
{
	uintmax_t			time;
	static const char	*colour[] = {BHRED, BHGRN, BHCYN, BHBLU, BHMAG, BHYEL};

	pthread_mutex_lock(&philo->arg->msg_lock);
	if (!philo_end(philo))
	{
		time = get_time_ms() - philo->arg->start_ms;
		printf("%s", colour[philo->seat % 6]);
		printf("%ju\tphilosopher [%d] %s.", time, philo->seat, message);
		if (philo->arg->eat_no && state == EAT)
			printf(" (%d/%d)", philo->times_eaten + 1, philo->arg->eat_no);
		printf("\n%s", BHWHT);
	}
	pthread_mutex_unlock(&philo->arg->msg_lock);
}
