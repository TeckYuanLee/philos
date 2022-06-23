/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   forks_philos.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: telee <telee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/23 18:49:52 by telee             #+#    #+#             */
/*   Updated: 2022/06/23 18:49:52 by telee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

void	philo_action(t_philo *philo)
{
	int	side;

	if (philo->seat % 2 == 0)
		side = LEFT;
	else
		side = RIGHT;
	pthread_mutex_lock(philo->hands[side]);
	update_state(philo, MSG_FORK, FORK);
	pthread_mutex_lock(philo->hands[!side]);
	update_state(philo, MSG_FORK, FORK);
	update_eaten_ms(philo);
	update_state(philo, MSG_EAT, EAT);
	usleep_chunks(philo->arg->eat_ms);
	philo->times_eaten++;
	pthread_mutex_unlock(philo->hands[LEFT]);
	pthread_mutex_unlock(philo->hands[RIGHT]);
	update_state(philo, MSG_SLEEP, SLEEP);
	usleep_chunks(philo->arg->sleep_ms);
	update_state(philo, MSG_THINK, THINK);
}

void	*philos_eaten_dead(t_philo *philo, t_state state)
{
	if (state == EATEN)
	{
		update_state(philo, MSG_EATEN, EATEN);
		philo->arg->philos_eaten++;
	}
	if (state == DEAD)
	{
		update_state(philo, MSG_DIED, DEAD);
		pthread_mutex_lock(&philo->arg->lock.dead);
		philo->arg->is_dead = true;
		pthread_mutex_unlock(&philo->arg->lock.dead);
		pthread_mutex_unlock(&philo->arg->lock.eat);
		pthread_mutex_unlock(philo->hands[LEFT]);
		pthread_mutex_unlock(philo->hands[RIGHT]);
	}
	return (NULL);
}

void	*philo_check(void *philo_arg)
{
	t_philo		*philo;

	philo = (t_philo *)philo_arg;
	while (1)
	{
		pthread_mutex_lock(&philo->arg->lock.eat);
		if (get_time_ms() > philo->deadline)
			return (philos_eaten_dead(philo, DEAD));
		pthread_mutex_unlock(&philo->arg->lock.eat);
		if (philo->arg->eat_no && philo_status(philo, EATEN))
			return (philos_eaten_dead(philo, EATEN));
		usleep(1000);
	}
	return (NULL);
}

void	*philo_start(void *philo_arg)
{
	t_philo		*philo;
	pthread_t	tid;

	philo = (t_philo *)philo_arg;
	if (pthread_create(&tid, NULL, &philo_check, philo_arg))
		return ((void *)1);
	if (philo->seat % 2 == 0)
		usleep(1000);
	update_eaten_ms(philo);
	while (!philo_status(philo, END))
	{
		philo_action(philo);
		usleep(1000);
	}
	pthread_join(tid, NULL);
	return (NULL);
}

int	create_threads(t_arg *args, t_philo *philos)
{
	int	i;

	i = -1;
	while (philos && ++i < args->philos)
	{
		if (pthread_create(&philos[i].id, NULL, &philo_start, &philos[i]))
			return (1);
	}
	i = -1;
	while (++i < args->philos)
		pthread_join(philos[i].id, NULL);
	return (0);
}
