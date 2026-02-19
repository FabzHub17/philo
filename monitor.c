#include "philo.h"

static void	set_dead(t_philo *philo)
{
	pthread_mutex_lock(&philo->rules->death_mutex);
	philo->rules->dead = 1;
	pthread_mutex_unlock(&philo->rules->death_mutex);
}

static int	check_death(t_philo *philos)
{
	int		i;
	long	time_since_meal;
	t_rules	*rules;

	i = 0;
	rules = philos[0].rules;
	while (i < rules->num_of_philo)
	{
		pthread_mutex_lock(&philos[i].meal_mutex);
		time_since_meal = get_time_ms() - philos[i].time_of_last_meal;
		pthread_mutex_unlock(&philos[i].meal_mutex);
		if (time_since_meal >= rules->time_die)
		{
			set_dead(&philos[i]);
			print_state(&philos[i], "died");
			return (1);
		}
		i++;
	}
	return (0);
}

static int	check_all_ate(t_philo *philos)
{
	int		i;
	t_rules	*rules;

	rules = philos[0].rules;
	if (rules->num_must_eat == -1)
		return (0);
	i = 0;
	while (i < rules->num_of_philo)
	{
		pthread_mutex_lock(&philos[i].meal_mutex);
		if (philos[i].meals_eaten < rules->num_must_eat)
		{
			pthread_mutex_unlock(&philos[i].meal_mutex);
			return (0);
		}
		pthread_mutex_unlock(&philos[i].meal_mutex);
		i++;
	}
	set_dead(&philos[0]);
	return (1);
}

void	*monitor_routine(void *arg)
{
	t_philo	*philos;

	philos = (t_philo *)arg;
	while (1)
	{
		if (check_death(philos) || check_all_ate(philos))
			return (NULL);
		usleep(500);
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
Quando tutti hanno mangiato abbastanza, usiamo set_dead(&philos[0]) — 
passiamo il primo filosofo solo per accedere a rules. Non stiamo dicendo 
che il filosofo 1 è morto, stiamo solo settando il flag globale rules->dead = 1 
per fermare la simulazione. I filosofi nel loro loop controllano questo flag e si fermano da soli.

Ordine critico in check_death
Prima lockiamo meal_mutex, leggiamo time_of_last_meal, poi unlockiamo — 
poi chiamiamo set_dead che locka death_mutex. Non teniamo mai due mutex 
lockati contemporaneamente nello stesso thread: questo evita potenziali 
deadlock tra monitor e filosofi.

*/