/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: telee <telee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/23 18:49:47 by telee             #+#    #+#             */
/*   Updated: 2022/06/23 18:49:47 by telee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

void	assign_forks(int i, t_philo *philo, t_arg *args)
{
	philo->hands[LEFT] = &(args->lock.forks[i]);
	if (i == philo->arg->philos - 1)
		philo->hands[RIGHT] = &(args->lock.forks[0]);
	else
		philo->hands[RIGHT] = &(args->lock.forks[i + 1]);
}

int	init_philos(t_arg *args, t_philo **philos)
{
	int	i;

	*philos = NULL;
	*philos = malloc(sizeof(t_philo) * args->philos);
	if (*philos == NULL)
		return (1);
	i = -1;
	while (++i < args->philos)
	{
		(*philos)[i].id = 0;
		(*philos)[i].seat = i + 1;
		(*philos)[i].times_eaten = 0;
		(*philos)[i].eaten_ms = get_time_ms();
		(*philos)[i].deadline = (*philos)[i].eaten_ms + args->die_ms;
		(*philos)[i].arg = args;
		assign_forks(i, &(*philos)[i], args);
	}
	args->init.philos = true;
	return (0);
}

int	init_locks(t_arg *args)
{
	int	i;

	if (pthread_mutex_init(&args->lock.msg, NULL))
		return (1);
	if (pthread_mutex_init(&args->lock.dead, NULL))
		return (1);
	if (pthread_mutex_init(&args->lock.eat, NULL))
		return (1);
	args->lock.forks = malloc(sizeof(pthread_mutex_t) * args->philos);
	if (args->lock.forks == NULL)
		return (1);
	i = -1;
	while (++i < args->philos)
	{
		if (pthread_mutex_init(&(args->lock.forks[i]), NULL))
			return (1);
	}
	args->init.fork = true;
	return (0);
}

int	init_args(t_arg *args, char **argv)
{
	args->philos = 0;
	args->die_ms = 0;
	args->eat_ms = 0;
	args->sleep_ms = 0;
	args->start_ms = get_time_ms();
	args->eat_no = 0;
	args->philos_eaten = 0;
	args->is_dead = false;
	args->init.fork = false;
	args->init.philos = false;
	args->philos = ft_atoi(argv[0]);
	args->die_ms = ft_atoi(argv[1]);
	args->eat_ms = ft_atoi(argv[2]);
	args->sleep_ms = ft_atoi(argv[3]);
	args->eat_no = ft_atoi(argv[4]);
	if (args->eat_no == 0)
		return (printf(ERR_ARGS ERR_MS) | free_exit(args, NULL));
	else if (!(argv[4]))
		args->eat_no = 0;
	if (args->philos < 1 || args->philos > 200 || args->die_ms < 60
		|| args->eat_ms < 60 || args->sleep_ms < 60 || args->eat_no < 0)
		return (printf(ERR_ARGS ERR_MS) | free_exit(args, NULL));
	return (0);
}

int	main(int argc, char **argv)
{
	t_arg	args;
	t_philo	*philos;

	if (argc > 4 && argc < 7)
	{
		init_args(&args, argv + 1);
		if (init_locks(&args) || init_philos(&args, &philos))
			return (printf(ERR_GEN) | free_exit(&args, &philos));
		if (create_threads(&args, philos))
			return (printf(ERR_GEN) | free_exit(&args, &philos));
		free_exit(&args, &philos);
	}
	else
		return (printf(ERR_INPUT));
	return (0);
}
