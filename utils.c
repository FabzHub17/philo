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
Perché il check !philo->rules->dead dentro print_state?
Evita che messaggi di "is thinking" o "is eating" vengano
stampati dopo il messaggio di morte. Il print_mutex garantisce 
che solo un thread alla volta stampi, e il check sul flag blocca i 
messaggi tardivi. Pulito e thread-safe.
*/
void	print_state(t_philo *philo, char *msg)
{
	long	timestamp;

	pthread_mutex_lock(&philo->rules->print_mutex);
	if (!philo->rules->dead)
	{
		timestamp = get_time_ms() - philo->rules->start_time;
		printf("%ld %d %s\n", timestamp, philo->id, msg);
	}
	pthread_mutex_unlock(&philo->rules->print_mutex);
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

/*
** Versione migliorata di ft_atoi che:
** 1. Rifiuta input non numerici
** 2. Gestisce overflow
** 3. Controlla che l'intera stringa sia valida
*/
int	ft_parsed_atoi(const char *str, int *result)
{
	long	num;
	int		sign;

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
		return (1);  // Errore: primo carattere non è numero
	while (is_digit(*str))
	{
		if (check_overflow(num, *str, sign))
			return (1);  // Errore: overflow
		num = num * 10 + (*str - '0');
		str++;
	}
	// Dopo i numeri, devono finire gli spazi o finire la stringa
	while (is_space(*str))
		str++;
	if (*str != '\0')
		return (1);  // Errore: caratteri non validi dopo i numeri
	
	*result = (int)(num * sign);
	return (0);
}


/* UTILS.C  
get_time_ms → restituisce il timestamp corrente in millisecondi
ft_usleep → sleep preciso (usleep nativo è inaffidabile per tempi lunghi)
print_state → stampa thread-safe degli stati
*/
