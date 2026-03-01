/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tvithara <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 16:31:42 by tvithara          #+#    #+#             */
/*   Updated: 2026/02/25 16:31:46 by tvithara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
** Validazione input aggiunta
*/
static int	validate_args(t_table *table)
{
	if (table->num_of_philo <= 0 || table->num_of_philo > 200)
		return (0);
	if (table->time_die <= 0 || table->time_eat <= 0 || table->time_sleep <= 0)
		return (0);
	if (table->num_must_eat != -1 && table->num_must_eat <= 0)
		return (0);
	return (1);
}

static int	init_forks(t_table *table)
{
	int	i;

	table->forks = malloc(sizeof(pthread_mutex_t) * table->num_of_philo);
	if (!table->forks)
		return (0);
	i = 0;
	while (i < table->num_of_philo)
	{
		if (pthread_mutex_init(&table->forks[i], NULL) != 0)
		{
			while (--i >= 0)
				pthread_mutex_destroy(&table->forks[i]);
			free(table->forks);
			return (0);
		}
		i++;
	}
	return (1);
}

static int	check_set_args(t_table *table, int ac, char **av)
{
	int	i;

	i = 1;
	while (i < ac)
	{
		if (custom_atoi(av[i]) == -1)
		{
			printf("Input Error: all arguments must be positive integers\n");
			return (0);
		}
		i++;
	}
	table->num_of_philo = custom_atoi(av[1]);
	table->time_die = custom_atoi(av[2]);
	table->time_eat = custom_atoi(av[3]);
	table->time_sleep = custom_atoi(av[4]);
	table->num_must_eat = -1;
	if (ac == 6)
		table->num_must_eat = custom_atoi(av[5]);
	return (1);
}

int	init_args(t_table *table, int ac, char **av)
{
	if (!check_set_args(table, ac, av))
		return (0);
	if (!validate_args(table))
		return (0);
	table->dead = 0;
	table->start_time = get_time_ms();
	if (pthread_mutex_init(&table->death_mutex, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&table->print_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&table->death_mutex);
		return (0);
	}
	if (!init_forks(table))
	{
		pthread_mutex_destroy(&table->death_mutex);
		pthread_mutex_destroy(&table->print_mutex);
		return (0);
	}
	return (1);
}
