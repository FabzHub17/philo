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
** set_dead: setta il flag dead in modo thread-safe.
** Usato da check_all_ate quando tutti hanno mangiato abbastanza.
*/
static void	set_dead(t_table *table)
{
    pthread_mutex_lock(&table->death_mutex);
    table->dead = 1;
    pthread_mutex_unlock(&table->death_mutex);
}

/*
** check_death: controlla se un filosofo è morto di fame.
**
** ORDINE DEI LOCK CRITICO per evitare data race con print_state:
** print_state fa: lock(print_mutex) → lock(death_mutex)
** check_death fa: lock(print_mutex) → lock(death_mutex)
** Stesso ordine → nessun deadlock, nessuna race su table->dead.
**
** Perché lock(print_mutex) qui?
** Vogliamo che "died" sia l'UNICO messaggio dopo la morte.
** Prendendo print_mutex, blocchiamo tutti i filosofi che vogliono
** stampare, settiamo dead = 1, stampiamo "died", poi rilasciamo.
** Nessun filosofo può inserire un messaggio tra "died" e dead=1.
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
            pthread_mutex_lock(&table->print_mutex);
            pthread_mutex_lock(&table->death_mutex);
            table->dead = 1;
            pthread_mutex_unlock(&table->death_mutex);
            printf("%ld %d died\n",
                get_time_ms() - table->start_time, philos[i].id);
            pthread_mutex_unlock(&table->print_mutex);
			return (1);
		}
		i++;
	}
	return (0);
}

/*
** check_all_ate: ritorna 1 se tutti i filosofi hanno raggiunto
** num_must_eat pasti. Setta dead = 1 per fermare i filosofi.
** Non stampa "died" perché non è una morte — è la fine normale.
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
	/*
	** Piccola pausa iniziale: i filosofi hanno bisogno di un attimo
	** per partire e aggiornare time_of_last_meal prima che il monitor
	** inizi a controllare. 1ms è sufficiente.
	*/
	usleep(1000);
	while (1)
	{
		if (check_death(philos) || check_all_ate(philos))
			return (NULL);
		usleep(500);
	}
	return (NULL);
}



