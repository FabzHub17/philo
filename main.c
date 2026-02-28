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
** launch_philos: crea i thread dei filosofi.
** NON facciamo più pthread_detach!
**
** Perché il cambio da detach a join?
** Prima: detach + usleep(5000) nel main → Helgrind vedeva i thread
** ancora vivi che accedevano a mutex già distrutti dal cleanup → DATA RACE!
**
** Ora: join_philos aspetta che TUTTI i thread finiscano davvero.
** I thread finiscono perché ft_usleep controlla is_dead() ogni 500µs.
** Quando il monitor setta dead = 1, entro ~1ms tutti i filosofi
** escono dal loro loop → il join ritorna → cleanup sicuro → zero race.
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
		i++;
	}
	return (1);
}

/*
** join_philos: aspetta che tutti i thread dei filosofi finiscano.
** Questo è il cuore della soluzione ai data race di Helgrind.
** Solo DOPO che tutti hanno terminato, facciamo cleanup dei mutex.
*/
static void	join_philos(t_table *table, t_philo *philos)
{
	int	i;

	i = 0;
	while (i < table->num_of_philo)
	{
		pthread_join(philos[i].thread, NULL);
		i++;
	}
}

int main(int ac, char **av)
{
	t_table table;
	t_philo *philos;
	pthread_t monitor;

	if (ac < 5 || ac > 6)
	{
		printf("Error: wrong number of arguments\n");
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
	** Prima lanciamo il monitor, poi aspettiamo lui.
	** Il monitor termina quando: un filosofo muore, o tutti hanno mangiato.
	** Dopo il join del monitor, i filosofi stanno già uscendo (is_dead = 1).
	** join_philos aspetta che abbiano finito davvero → poi cleanup.
	*/
	if (pthread_create(&monitor, NULL, monitor_routine, philos) != 0)
	{
		cleanup(&table, philos);
		printf("Error: monitor thread creation failed\n");
		return (1);
	}
	pthread_join(monitor, NULL);
	join_philos(&table, philos);
	/*
	** A questo punto: monitor terminato + tutti i filosofi terminati.
	** I mutex non sono più usati da nessuno → cleanup sicuro.
	*/
	//usleep(5000);
	cleanup(&table, philos);
	return (0);

}
