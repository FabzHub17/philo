#include "philo.h"

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
	if (sign == -1 && (-next < -2147483648))
		return (1);
	return (0);
}

static void	sign_check(char **s, int *sign)
{
	if (**s == '+' || **s == '-')
	{
		if (**s == '-')
			*sign = -1;
		(*s)++;
	}
}

int	custom_atoi(char *str)
{
	long	num;
	int		sign;
	int		res;

	num = 0;
	sign = 1;
	while (is_space(*str))
		str++;
	sign_check(&str, &sign);
	if (!is_digit(*str))
		return (-1);
	while (is_digit(*str))
	{
		if (check_overflow(num, *str, sign))
			return (-1);
		num = num * 10 + (*str - '0');
		str++;
	}
	while (is_space(*str))
		str++;
	if (*str != '\0')
		return (-1);
	res = (num * sign);
	return (res);
}
