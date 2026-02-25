/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tvithara <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 16:31:18 by tvithara          #+#    #+#             */
/*   Updated: 2026/02/25 16:31:23 by tvithara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	destroy_forks(t_rules *rules)
{
	int	i;

	i = 0;
	while (i < rules->num_of_philo)
	{
		pthread_mutex_destroy(&rules->forks[i]);
		i++;
	}
	free(rules->forks);
}

static void	destroy_philos(t_rules *rules, t_philo *philos)
{
	int	i;

	i = 0;
	while (i < rules->num_of_philo)
	{
		pthread_mutex_destroy(&philos[i].meal_mutex);
		i++;
	}
	free(philos);
}

void	cleanup(t_rules *rules, t_philo *philos)
{
	destroy_forks(rules);
	destroy_philos(rules, philos);
	pthread_mutex_destroy(&rules->death_mutex);
	pthread_mutex_destroy(&rules->print_mutex);
}

/*
Ordine del cleanup è importante
Prima distruggi le forchette e i meal_mutex, poi death_mutex e print_mutex. 
Il motivo è che fino all'ultimo momento i thread potrebbero ancora usare 
death_mutex e print_mutex — dopo il join dei thread nel main sei sicuro che 
nessuno li stia usando più, quindi è safe distruggerli.
*/
