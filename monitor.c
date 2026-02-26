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

static void	set_dead(t_table *table)
{
    pthread_mutex_lock(&table->death_mutex);
    table->dead = 1;
    pthread_mutex_unlock(&table->death_mutex);
}

/*
** Migliorato: ora passiamo rules invece di un filosofo specifico
*/
static int	check_death(t_philo *philos)
{
	int		i;
	long	time_since_meal;
	t_table	*table;

	i = 0;
	table = philos[0].table;
	while (i < table->num_of_philo)
	{
		pthread_mutex_lock(&philos[i].meal_mutex);
		time_since_meal = get_time_ms() - philos[i].time_of_last_meal;
		pthread_mutex_unlock(&philos[i].meal_mutex);
		
		if (time_since_meal > table->time_die)  // > invece di >= per sicurezza
		{
			// IMPORTANTE: stampa DOPO aver settato dead, così print_state funziona
			print_state(&philos[i], "died");
			set_dead(table);
			return (1);
		}
		i++;
	}
	return (0);
}

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
	// Tutti hanno mangiato abbastanza, ma non sono "morti" — usiamo set_dead solo per fermare la simulazione
	set_dead(table);  // settiamo dead per fermare i filosofi, anche se non sono "morti"
	return (1);
}

void	*monitor_routine(void *arg)
{
	t_philo	*philos;

	philos = (t_philo *)arg;
	// Piccolo ritardo iniziale per dare tempo ai filosofi di partire
	usleep(1000);
	while (1)
	{
		if (check_death(philos) || check_all_ate(philos))
			return (NULL);
		usleep(500); // Controllo ogni 500µs (entro i 10ms richiesti)
	}
	return (NULL);
}


/*

Perché usleep(500) nel loop del monitor?
Il monitor non deve girare a piena velocità — spreca CPU inutilmente. 
500 microsecondi è un buon intervallo: abbastanza frequente da rilevare la 
morte entro i 10ms richiesti dal subject, abbastanza lento da non martellare 
il processore.

Il trucco di set_dead nel caso check_all_ate
Quando tutti hanno mangiato abbastanza, usiamo set_dead(table) — 
passiamo table direttamente. Non stiamo dicendo 
che il filosofo 1 è morto, stiamo solo settando il flag globale table->dead = 1 
per fermare la simulazione. I filosofi nel loro loop controllano questo flag e si fermano da soli.

Ordine critico in check_death
Prima lockiamo meal_mutex, leggiamo time_of_last_meal, poi unlockiamo — 
poi chiamiamo set_dead che locka death_mutex. Non teniamo mai due mutex 
lockati contemporaneamente nello stesso thread: questo evita potenziali 
deadlock tra monitor e filosofi.

*/
