/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: telee <telee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/23 18:49:55 by telee             #+#    #+#             */
/*   Updated: 2022/06/23 18:49:55 by telee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

void	update_state(t_philo *philo, char *message, t_state state)
{
	uintmax_t		time;
	static char		*colour[] = {BHRED, BHGRN, BHCYN, BHBLU, BHMAG, BHYEL};

	pthread_mutex_lock(&philo->arg->lock.msg);
	if (!philo_status(philo, DEAD))
	{
		time = get_time_ms() - philo->arg->start_ms;
		printf("%s", colour[philo->seat % 6]);
		printf("%ju Philosophers [%d]%s", time, philo->seat, message);
		if (philo->arg->eat_no && state == EAT)
			printf(" (%d/%d)", philo->times_eaten + 1, philo->arg->eat_no);
		printf("\n%s", BHWHT);
	}
	pthread_mutex_unlock(&philo->arg->lock.msg);
}

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

uintmax_t	get_time_ms(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return ((time.tv_sec * 1000) + (time.tv_usec / 1000));
}

bool	philo_status(t_philo *philo, t_state state)
{
	bool	done;

	if (state == EATEN || state == END)
	{
		done = false;
		pthread_mutex_lock(&philo->arg->lock.eat);
		if (philo->arg->eat_no
			&& philo->times_eaten == philo->arg->eat_no)
			done = true;
		pthread_mutex_unlock(&philo->arg->lock.eat);
		if (state == EATEN)
			return (done);
	}
	if (state == DEAD || (state == END && done == false))
	{
		done = false;
		pthread_mutex_lock(&philo->arg->lock.dead);
		if (philo->arg->is_dead
			|| philo->arg->philos_eaten == philo->arg->philos)
			done = true;
		pthread_mutex_unlock(&philo->arg->lock.dead);
	}
	return (done);
}
