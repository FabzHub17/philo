#include "philo.h"

static void	assign_forks(t_table *table, t_philo **philos, int i, int n)
{
	if (i % 2 == 0)
	{
		(*philos)[i].left_fork = &table->forks[i];
		(*philos)[i].right_fork = &table->forks[(i + 1) % n];
	}
	else
	{
		(*philos)[i].left_fork = &table->forks[(i + 1) % n];
		(*philos)[i].right_fork = &table->forks[i];
	}
}

int	init_philos(t_table *table, t_philo **philos)
{
	int	i;
	int	n;

	n = table->num_of_philo;
	*philos = malloc(sizeof(t_philo) * n);
	if (!*philos)
		return (0);
	i = 0;
	while (i < n)
	{
		(*philos)[i].id = i + 1;
		(*philos)[i].meals_eaten = 0;
		(*philos)[i].time_of_last_meal = table->start_time;
		(*philos)[i].table = table;
		if (pthread_mutex_init(&(*philos)[i].meal_mutex, NULL) != 0)
		{
			while (--i >= 0)
				pthread_mutex_destroy(&(*philos)[i].meal_mutex);
			free(*philos);
			return (0);
		}
		assign_forks(table, philos, i, n);
		i++;
	}
	return (1);
}
