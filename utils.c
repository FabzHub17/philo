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

int	ft_atoi(const char *str)
{
	int	result;

	result = 0;
	while (*str == ' ' || (*str >= 9 && *str <= 13))
		str++;
	if (*str == '+')
		str++;
	while (*str >= '0' && *str <= '9')
		result = result * 10 + (*str++ - '0');
	return (result);
}


/* UTILS.C  
get_time_ms → restituisce il timestamp corrente in millisecondi
ft_usleep → sleep preciso (usleep nativo è inaffidabile per tempi lunghi)
print_state → stampa thread-safe degli stati
*/