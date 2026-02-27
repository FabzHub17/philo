/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tvithara <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 16:32:09 by tvithara          #+#    #+#             */
/*   Updated: 2026/02/25 16:32:11 by tvithara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
** Setta il flag globale di morte nella tabella.
** Usiamo death_mutex per evitare data race con i filosofi
** che leggono questo flag nel loro loop (is_dead).
*/
static void	set_dead(t_table *table)
{
    pthread_mutex_lock(&table->death_mutex);
    table->dead = 1;
    pthread_mutex_unlock(&table->death_mutex);
}

/*
** ORDINE CRITICO PER "died":
** 1. Prima settiamo dead = 1  → i filosofi smettono di stampare
** 2. Poi stampiamo "died"     → unico messaggio dopo la morte
**
** Perché questo ordine? print_state controlla !table->dead prima di
** stampare. Se setto dead DOPO aver stampato, un filosofo potrebbe
** stampare "is eating" tra il nostro "died" e il nostro set_dead → casino!
** Settando dead PRIMA, siamo sicuri che solo NOI stampiamo "died" e
** nessun altro filosofo stamperà più nulla.
*/
static int	check_death(t_philo *philos)
{
	int		i;
	long	time_since_meal;
	t_table	*table;

	i = 0;
	table = philos[0].table;  // Tutti i filosofi condividono la stessa tabella, prendo la prima
	while (i < table->num_of_philo)
	{
		pthread_mutex_lock(&philos[i].meal_mutex);
		time_since_meal = get_time_ms() - philos[i].time_of_last_meal;
		pthread_mutex_unlock(&philos[i].meal_mutex);
		
		if (time_since_meal > table->time_die)  // > invece di >= per sicurezza
		{
			set_dead(table);
			pthread_mutex_lock(&table->print_mutex);
    		printf("%ld %d died\n", get_time_ms() - table->start_time, philos[i].id);
    		pthread_mutex_unlock(&table->print_mutex);
			//print_state(&philos[i], "died");
			return (1);
		}
		i++;
	}
	return (0);
}

/*
** Controlla se tutti i filosofi hanno mangiato abbastanza.
** Ritorna 1 (e stoppa la sim) solo se num_must_eat è specificato
** e TUTTI hanno raggiunto il numero richiesto.
*/
static int	check_all_ate(t_philo *philos)
{
	int		i;
	t_table	*table;

	table = philos[0].table;
	if (table->num_must_eat == -1)
		return (0);
	i = 0;
	while (i < table->num_of_philo)
	{
		pthread_mutex_lock(&philos[i].meal_mutex);
		if (philos[i].meals_eaten < table->num_must_eat)
		{
			pthread_mutex_unlock(&philos[i].meal_mutex);
			return (0);
		}
		pthread_mutex_unlock(&philos[i].meal_mutex);
		i++;
	}
	set_dead(table);  // ma cosi non stampa "died" se tutti hanno mangiato? No, perché non è un decesso, è solo la fine della simulazione. I filosofi si fermeranno da soli quando vedranno dead = 1.
	return (1);
}

void	*monitor_routine(void *arg)
{
	t_philo	*philos;

	philos = (t_philo *)arg;
	usleep(1000);
	while (1)
	{
		if (check_death(philos) || check_all_ate(philos))
			return (NULL);
		usleep(500);
	}
	return (NULL);
}



