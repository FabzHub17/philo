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
static int validate_input(t_rules *rules)
{
	if (rules->num_of_philo <= 0 || rules->num_of_philo > 200) // perchè 200?
		return (1);
	if (rules->time_die <= 0 || rules->time_eat <= 0 || rules->time_sleep <= 0)
		return (1);
	if (rules->num_must_eat != -1 && rules->num_must_eat <= 0)  // num_must_eat può essere -1 (non specificato) o > 0
		return (1);
	return (0);
}

static int init_forks(t_rules *rules)
{
	int i;

	rules->forks = malloc(sizeof(pthread_mutex_t) * rules->num_of_philo);
	if (!rules->forks)
		return (1);
	i = 0;
	while (i < rules->num_of_philo)
	{
		if (pthread_mutex_init(&rules->forks[i], NULL) != 0) // da controllare
		{
			// Cleanup parziale in caso di errore
			while (--i >= 0)
				pthread_mutex_destroy(&rules->forks[i]);
			free(rules->forks);
			return (1);
		}
		i++;
	}
	return (0);
}

int init_rules(t_rules *rules, int ac, char **av)
{
	rules->num_of_philo = ft_atoi(av[1]);
	rules->time_die = ft_atoi(av[2]);
	rules->time_eat = ft_atoi(av[3]);
	rules->time_sleep = ft_atoi(av[4]);
	rules->num_must_eat = -1;
	if (ac == 6)
		rules->num_must_eat = ft_atoi(av[5]);

	if (validate_input(rules))
		return (1);

	rules->dead = 0;
	rules->start_time = get_time_ms();

	if (pthread_mutex_init(&rules->death_mutex, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&rules->print_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&rules->death_mutex);
		return (1);
	}
	if (init_forks(rules))
	{
		pthread_mutex_destroy(&rules->death_mutex);
		pthread_mutex_destroy(&rules->print_mutex);
		return (1);
	}
	return (0);
}

int init_philos(t_rules *rules, t_philo **philos)
{
	int i;
	int n;

	n = rules->num_of_philo;
	*philos = malloc(sizeof(t_philo) * n);
	if (!*philos)
		return (1);
	i = 0;
	while (i < n)
	{
		(*philos)[i].id = i + 1;
		(*philos)[i].meals_eaten = 0;
		(*philos)[i].time_of_last_meal = rules->start_time;
		(*philos)[i].rules = rules;

		if (pthread_mutex_init(&(*philos)[i].meal_mutex, NULL) != 0)
		{
			// Cleanup filosofi già inizializzati
			while (--i >= 0)
				pthread_mutex_destroy(&(*philos)[i].meal_mutex);
			free(*philos);
			return (1);
		}

		// Assegnazione forchette (questa parte è già corretta)
		if (i % 2 == 0)
		{
			(*philos)[i].left_fork = &rules->forks[i];
			(*philos)[i].right_fork = &rules->forks[(i + 1) % n];
		}
		else
		{
			(*philos)[i].left_fork = &rules->forks[(i + 1) % n];
			(*philos)[i].right_fork = &rules->forks[i];
		}
		i++;
	}
	return (0);
}
