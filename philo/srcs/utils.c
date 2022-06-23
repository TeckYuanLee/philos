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

int	free_exit(t_arg *args, t_philo **philos)
{
	int	i;

	i = -1;
	if (args != NULL)
	{
		pthread_mutex_destroy(&args->lock.msg);
		pthread_mutex_destroy(&args->lock.dead);
		pthread_mutex_destroy(&args->lock.eat);
	}
	if (philos != NULL && args->init.philos)
	{
		while (++i < args->philos)
			pthread_mutex_destroy(((*philos)[i].hands[LEFT]));
		free(*philos);
	}
	if (args->init.fork)
		free(args->lock.forks);
	exit(0);
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
