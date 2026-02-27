/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tvithara <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 16:33:25 by tvithara          #+#    #+#             */
/*   Updated: 2026/02/25 16:33:28 by tvithara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long	get_time_ms(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000L + time.tv_usec / 1000);
}

/*
Perché ft_usleep e non usleep direttamente?
usleep su Linux può dormire più del dovuto 
— il kernel non garantisce precisione al millisecondo.
Con il loop che controlla il tempo ogni 500 microsecondi, 
sei molto più preciso. 500µs è un buon compromesso tra precisione 
e consumo CPU.
*/
void	ft_usleep(long ms)
{
	long	start;

	start = get_time_ms();
	while (get_time_ms() - start < ms)
		usleep(500);
}


/*
** print_state: stampa thread-safe dello stato del filosofo.
**
** IMPORTANTE: per leggere table->dead usiamo death_mutex,
** lo stesso mutex che usa set_dead in monitor.c.
** Prima, il vecchio codice leggeva dead dentro print_mutex →
** due mutex diversi per la stessa variabile = DATA RACE!
** Ora: lock death_mutex per leggere dead, poi se ok
** lock print_mutex per stampare. Pulito e corretto.
*/
void	print_state(t_philo *philo, char *msg)
{
	long	timestamp;
	int		is_simulation_dead;

	// controlla se la simulazione è finita — se sì, non stampare nulla
	pthread_mutex_lock(&philo->table->death_mutex);
	is_simulation_dead = philo->table->dead;
	pthread_mutex_unlock(&philo->table->death_mutex);
	if (is_simulation_dead)
		return ;
	// stampa protetta da print_mutex per evitare messaggi sovrapposti
	pthread_mutex_lock(&philo->table->print_mutex);
	timestamp = get_time_ms() - philo->table->start_time;
	printf("%ld %d %s\n", timestamp, philo->id, msg);
	pthread_mutex_unlock(&philo->table->print_mutex);
}

static int	is_digit(char c)
{
	return (c >= '0' && c <= '9');
}

static int	is_space(char c)
{
	return (c == ' ' || (c >= 9 && c <= 13));
}

static int	check_overflow(long num, char c, int sign)
{
	long	next;

	next = num * 10 + (c - '0');
	if (sign == 1 && next > 2147483647)
		return (1);
	if (sign == -1 && -next < -2147483648)
		return (1);
	return (0);
}

int	custom_atoi(char *str)
{
	long	num;
	int		sign;
	int    res;

	num = 0;
	sign = 1;
	while (is_space(*str))
		str++;
	if (*str == '+' || *str == '-')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}
	if (!is_digit(*str))
		return (-1);  
	while (is_digit(*str))
	{
		if (check_overflow(num, *str, sign))
			return (-1);  // Errore: overflow
		num = num * 10 + (*str - '0');
		str++;
	}
	// Dopo i numeri, devono finire gli spazi o finire la stringa
	while (is_space(*str))
		str++;
	if (*str != '\0')
		return (-1);  // Errore: caratteri non validi dopo i numeri
	
	res = (num * sign);
	return (res);
}

