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

static int is_dead(t_philo *philo)
{
    pthread_mutex_lock(&philo->table->death_mutex);
    if (philo->table->dead)
    {
        pthread_mutex_unlock(&philo->table->death_mutex);
        return (1);
    }
    pthread_mutex_unlock(&philo->table->death_mutex);
    return (0);
}

/*
** Versione per filosofi pari: left -> right
*/
static void philo_eat_normal(t_philo *philo)
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
}

/*
** Versione per filosofi dispari: right -> left
** EVITA IL DEADLOCK!
*/
static void philo_eat_reverse(t_philo *philo)
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
}

static void philo_sleep_think(t_philo *philo)
{
    print_state(philo, "is sleeping");
    ft_usleep(philo->table->time_sleep);
    print_state(philo, "is thinking");
}

/*
** Versione corretta per filosofo solitario
** Ora aspetta finché il monitor non lo dichiara morto
*/
static void lone_philo(t_philo *philo)
{
    pthread_mutex_lock(philo->left_fork);
    print_state(philo, "has taken a fork");
    
    // Il monitor rileverà la morte e stamperà "died"
    while (!is_dead(philo))
        usleep(500);
        
    pthread_mutex_unlock(philo->left_fork);
}

void *philo_routine(void *arg)
{
    t_philo *philo;

    philo = (t_philo *)arg;
    
    // Caso speciale: un solo filosofo
    if (philo->table->num_of_philo == 1)
    {
        lone_philo(philo);
        return (NULL);
    }
    
    // Sfasamento iniziale per ridurre contention
    if (philo->id % 2 == 0)
        ft_usleep(1);
    
    while (!is_dead(philo))
    {
        // SCELTA CRITICA: ordine delle forchette in base all'ID
        if (philo->id % 2 == 0)
            philo_eat_normal(philo);   // Pari: left -> right
        else
            philo_eat_reverse(philo);  // Dispari: right -> left
            
        if (is_dead(philo))
            break;
            
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

Il caso lone_philo
Con un solo filosofo c'è una sola forchetta —
non può mai mangiare. La gestione corretta è: prende la
forchetta, aspetta time_die + 1 ms (così il monitor ha il tempo
di rilevare la morte), poi rilascia. Senza questa funzione separata,
il filosofo solitario entrerebbe nel loop normale e si bloccherebbe su
left_fork per sempre senza mai morire "ufficialmente".

Nota importante su philo_eat
Aggiorniamo time_of_last_meal dopo aver preso
entrambe le forchette, non prima. Ha senso: il filosofo
inizia a mangiare solo quando ha entrambe le forchette in mano.
Il meal_mutex protegge questi valori dal monitor che li legge in
contemporanea.

*/
