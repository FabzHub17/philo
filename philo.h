/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tvithara <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/11 17:12:49 by tvithara          #+#    #+#             */
/*   Updated: 2025/10/11 17:13:48 by tvithara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
#define PHILO_H

#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

typedef struct s_rules
{
    int  num_of_philo;
    long time_die;
    long time_eat;
    long time_sleep;
    int  num_must_eat;

    long start_time;
    int  dead;

    pthread_mutex_t death_mutex;
    pthread_mutex_t print_mutex;
    pthread_mutex_t *forks;
}   t_rules;

typedef struct s_philo
{
    int id;
    int meals_eaten;
    long time_of_last_meal;

    pthread_t thread;
    pthread_mutex_t *left_fork;
    pthread_mutex_t *right_fork;
    pthread_mutex_t meal_mutex;     

    t_rules *rules;
}   t_philo;

// init.c
int	init_rules(t_rules *rules, int ac, char **av);
int	init_philos(t_rules *rules, t_philo **philos);
static int	init_forks(t_rules *rules);

// utils.c
long	get_time_ms(void);
void	ft_usleep(long ms);
void	print_state(t_philo *philo, char *msg);
int	ft_atoi(const char *str);

#endif