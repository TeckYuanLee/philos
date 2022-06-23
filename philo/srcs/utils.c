/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: telee <telee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/23 18:49:57 by telee             #+#    #+#             */
/*   Updated: 2022/06/23 18:49:57 by telee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

void	usleep_chunks(uintmax_t ms)
{
	uintmax_t		start;

	start = get_time_ms();
	while ((get_time_ms() - start) < ms)
		usleep(50);
}

void	update_eaten_ms(t_philo *philo)
{
	pthread_mutex_lock(&philo->arg->lock.eat);
	philo->eaten_ms = get_time_ms();
	philo->deadline = philo->eaten_ms + philo->arg->die_ms;
	pthread_mutex_unlock(&philo->arg->lock.eat);
}

int	ft_atoi(char *str)
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
