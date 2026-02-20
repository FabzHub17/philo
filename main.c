/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tvithara <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/11 17:12:36 by tvithara          #+#    #+#             */
/*   Updated: 2025/10/11 17:14:07 by tvithara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int main(int ac, char **av)
{
	t_rules rules;
	t_philo *philos;
	pthread_t monitor;
	int i;

	/* 
	CODICE ORIGINALE
	
	if (ac < 5 || ac > 6)
		return (1);
	if (init_rules(&rules, ac, av))
		return (1);
	if (init_philos(&rules, &philos))
		return (cleanup(&rules, philos), 1);
	i = 0;
	while (i < rules.num_of_philo)
	{
		pthread_create(&philos[i].thread, NULL, philo_routine, &philos[i]);
		i++;
	}
	pthread_create(&monitor, NULL, monitor_routine, philos);
	pthread_join(monitor, NULL);
	i = 0;
	while (i < rules.num_of_philo)
	{
		pthread_join(philos[i].thread, NULL);
		i++;
	}
	cleanup(&rules, philos);
	return (0); */

	if (ac < 5 || ac > 6)
	{
		printf("Error: wrong number of arguments\n");
		printf("Usage: ./philo number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]\n");
		return (1);
	}

	if (init_rules(&rules, ac, av))
	{
		printf("Error: initialization failed\n");
		return (1);
	}

	if (init_philos(&rules, &philos))
	{
		cleanup(&rules, philos);
		printf("Error: philosopher initialization failed\n");
		return (1);
	}

	i = 0;
	while (i < rules.num_of_philo)
	{
		if (pthread_create(&philos[i].thread, NULL, philo_routine, &philos[i]) != 0)
		{
			// In caso di errore, termina i thread già creati
			while (--i >= 0)
				pthread_join(philos[i].thread, NULL);
			cleanup(&rules, philos);
			printf("Error: thread creation failed\n");
			return (1);
		}
		i++;
	}

	pthread_create(&monitor, NULL, monitor_routine, philos);
	pthread_join(monitor, NULL);

	i = 0;
	while (i < rules.num_of_philo)
	{
		pthread_join(philos[i].thread, NULL);
		i++;
	}

	cleanup(&rules, philos);
	return (0);
}