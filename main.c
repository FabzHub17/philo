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

/*
** Lancia tutti i thread dei filosofi.
** Ritorna 0 se almeno un thread non si crea.
*/
/*
** DETACH subito dopo la creazione!
** Perché? Quando il monitor setta dead = 1 e termina,
** i filosofi potrebbero essere bloccati su pthread_mutex_lock
** (aspettando una forchetta). Se facessimo pthread_join,
** il main si bloccherebbe ad aspettare thread che non si
** sbloccheranno mai → il tester killa il programma → KO.
** Con detach, il thread è "libero": quando il processo termina,
** il sistema pulisce tutto automaticamente.
*/
static int	launch_philos(t_table *table, t_philo *philos)
{
	int	i;

	i = 0;
	while (i < table->num_of_philo)
	{
		if (pthread_create(&philos[i].thread, NULL,
				philo_routine, &philos[i]) != 0)
			return (0);
		pthread_detach(philos[i].thread);
		i++;
	}
	return (1);
}

int main(int ac, char **av)
{
	t_table table;
	t_philo *philos;
	pthread_t monitor;

	if (ac < 5 || ac > 6)
	{
		printf("Error: wrong number of arguments\n");
		printf("Usage: ./philo number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]\n");
		return (1);
	}
	if (!init_args(&table, ac, av))
	{
		printf("Error: invalid arguments\n");
		return (1);
	}
	if (!init_philos(&table, &philos))
	{
		cleanup(&table, philos);
		printf("Error: philosopher initialization failed\n");
		return (1);
	}
	if (!launch_philos(&table, philos))
	{
		cleanup(&table, philos);
		printf("Error: thread creation failed\n");
		return (1);
	}
	/*
	** Creiamo il thread monitor e aspettiamo SOLO lui.
	** Il monitor termina quando: un filosofo muore, o tutti hanno mangiato.
	** Dopo il join del monitor, il main fa cleanup e torna → processo termina.
	** I thread dei filosofi (detachati) vengono puliti dal sistema operativo.
	*/
	if (pthread_create(&monitor, NULL, monitor_routine, philos) != 0)
	{
		cleanup(&table, philos);
		printf("Error: monitor thread creation failed\n");
		return (1);
	}
	pthread_join(monitor, NULL);
	/*
	** Piccola pausa per dare tempo ai filosofi di stampare
	** eventuali messaggi in sospeso prima che cleanup distrugga i mutex.
	** 5ms è sufficiente senza impattare la precisione dei test.
	*/
	usleep(5000);
	cleanup(&table, philos);
	return (0);

}
