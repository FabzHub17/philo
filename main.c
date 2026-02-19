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

int main(int ac, char **av)
{
    t_rules rules;
    t_philo *philos;
    pthread_t monitor;

    if (ac < 5 || ac > 6)
        return (1);
    if (init_rules(&rules, ac, av))
        return (1);
    if (init_philos(&rules, &philos))
        return (1);
    
    pthread_create(&monitor, NULL, monitor_rotine, philos);
    pthread_detach(monitor);

    cleanup(&rules,philos);
    return (0);
}