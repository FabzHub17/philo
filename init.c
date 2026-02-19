#include "philo.h"

static int	init_forks(t_rules *rules)
{
	int	i;

	rules->forks = malloc(sizeof(pthread_mutex_t) * rules->num_of_philo);
	if (!rules->forks)
		return (1);
	i = 0;
	while (i < rules->num_of_philo)
	{
		if (pthread_mutex_init(&rules->forks[i], NULL))
			return (1);
		i++;
	}
	return (0);
}

int	init_rules(t_rules *rules, int ac, char **av)
{
	rules->num_of_philo = ft_atoi(av[1]);
	rules->time_die = ft_atoi(av[2]);
	rules->time_eat = ft_atoi(av[3]);
	rules->time_sleep = ft_atoi(av[4]);
	rules->num_must_eat = -1;
	if (ac == 6)
		rules->num_must_eat = ft_atoi(av[5]);
	rules->dead = 0;
	rules->start_time = get_time_ms();
	if (rules->num_of_philo < 1 || rules->time_die < 0
		|| rules->time_eat < 0 || rules->time_sleep < 0)
		return (1);
	pthread_mutex_init(&rules->death_mutex, NULL);
	pthread_mutex_init(&rules->print_mutex, NULL);
	if (init_forks(rules))
		return (1);
	return (0);
}

int	init_philos(t_rules *rules, t_philo **philos)
{
	int	i;
	int	n;

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
		pthread_mutex_init(&(*philos)[i].meal_mutex, NULL);
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