/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tvithara <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 16:33:09 by tvithara          #+#    #+#             */
/*   Updated: 2026/02/25 16:33:11 by tvithara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
** Versione per filosofi pari: left -> right
*/
/* static void philo_eat_normal(t_philo *philo)
{
    pthread_mutex_lock(philo->left_fork);
    print_state(philo, "has taken a fork");
    pthread_mutex_lock(philo->right_fork);
    print_state(philo, "has taken a fork");
    
    print_state(philo, "is eating");
    
    pthread_mutex_lock(&philo->meal_mutex);
    philo->time_of_last_meal = get_time_ms();
    philo->meals_eaten++;
    pthread_mutex_unlock(&philo->meal_mutex);
    
    ft_usleep(philo->table->time_eat);
    
    pthread_mutex_unlock(philo->left_fork);
    pthread_mutex_unlock(philo->right_fork);
} */

/*
** Versione per filosofi dispari: right -> left
** EVITA IL DEADLOCK!
*/
/* static void philo_eat_reverse(t_philo *philo)
{
    pthread_mutex_lock(philo->right_fork);
    print_state(philo, "has taken a fork");
    pthread_mutex_lock(philo->left_fork);
    print_state(philo, "has taken a fork");
    
    print_state(philo, "is eating");
    
    pthread_mutex_lock(&philo->meal_mutex);
    philo->time_of_last_meal = get_time_ms();
    philo->meals_eaten++;
    pthread_mutex_unlock(&philo->meal_mutex);
    
    ft_usleep(philo->table->time_eat);
    
    pthread_mutex_unlock(philo->right_fork);
    pthread_mutex_unlock(philo->left_fork);
} */

/*
** philo_eat: prende le due forchette nell'ordine corretto,
** aggiorna l'ultimo pasto, mangia, rilascia.
**
** L'ordine delle forchette dipende da assign_forks in init.c:
** - filosofi pari:    left → right
** - filosofi dispari: right → left  (che è left_fork = fork alta, right = bassa)
** Questo garantisce gerarchia globale sulle forchette → no deadlock.
**
** Aggiorniamo time_of_last_meal DOPO aver preso entrambe le forchette,
** che è il momento in cui inizia davvero a mangiare.
*/
static void	philo_eat(t_philo *philo)
{
	pthread_mutex_lock(philo->left_fork);
	print_state(philo, "has taken a fork");
	pthread_mutex_lock(philo->right_fork);
	print_state(philo, "has taken a fork");
	print_state(philo, "is eating");
	pthread_mutex_lock(&philo->meal_mutex);
	philo->time_of_last_meal = get_time_ms();
	philo->meals_eaten++;
	pthread_mutex_unlock(&philo->meal_mutex);
	ft_usleep(philo->table->time_eat, philo);
	pthread_mutex_unlock(philo->left_fork);
	pthread_mutex_unlock(philo->right_fork);
}


static void philo_sleep_think(t_philo *philo)
{
    print_state(philo, "is sleeping");
    ft_usleep(philo->table->time_sleep, philo);
    print_state(philo, "is thinking");
}

/*
** lone_philo: caso speciale con un solo filosofo.
** Ha solo una forchetta → non può mai mangiare → muore.
** Prende la forchetta e aspetta che il monitor rilevi la morte.
** ft_usleep con is_dead garantisce uscita pulita.
*/
static void	lone_philo(t_philo *philo)
{
	pthread_mutex_lock(philo->left_fork);
	print_state(philo, "has taken a fork");
	while (!is_dead(philo))
		usleep(500);
	pthread_mutex_unlock(philo->left_fork);
}

void *philo_routine(void *arg)
{
    t_philo *philo;

    philo = (t_philo *)arg;
    
    if (philo->table->num_of_philo == 1)
    {
        lone_philo(philo);
        return (NULL);
    }
    /* // Sfasamento iniziale per ridurre contention
    if (philo->id % 2 == 0)
        ft_usleep(1);
    while (!is_dead(philo))
    {
        if (philo->id % 2 == 0)
            philo_eat_normal(philo);   // Pari: left -> right
        else
            philo_eat_reverse(philo);  // Dispari: right -> left
        if (is_dead(philo))
            break;
        philo_sleep_think(philo);
    } */

    /*
	** Sfasamento per i filosofi pari: aspettano 1ms prima di partire.
	** I dispari vanno subito a prendere le forchette.
	** Questo riduce la contention iniziale: i dispari prendono
	** le loro forchette, i pari trovano forchette libere subito dopo.
	*/
	if (philo->id % 2 == 0)
		ft_usleep(1, philo);
	while (!is_dead(philo))
	{
		philo_eat(philo);
		if (is_dead(philo))
			break ;
		philo_sleep_think(philo);
	}

    return (NULL);
}

/*
Perché ft_usleep(1) per i filosofi pari?
È un piccolo trucco anti-deadlock aggiuntivo.
I filosofi dispari partono subito a prendere le forchette,
quelli pari aspettano 1ms — così non si accalcano tutti sulle
stesse forchette all'inizio della simulazione. Con la gerarchia da
sola funziona già, ma questo rende tutto più fluido e riduce la contention.
*/
