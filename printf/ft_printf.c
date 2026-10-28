/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cornguye <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/17 12:34:40 by cornguye          #+#    #+#             */
/*   Updated: 2023/11/30 17:36:51 by cornguye         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "ft_printf.h"

static int	ft_condition(char str, va_list args)
{
	int	count;

	count = 0;
	if (str == 'c')
		count += ft_putchar(va_arg(args, int));
	else if (str == 's')
		count += ft_putstr(va_arg(args, char *));
	else if (str == 'd')
		count += ft_putnbr(va_arg(args, int));
	else if (str == 'i')
		count += ft_putnbr(va_arg(args, int));
	else if (str == '%')
	{
		count++;
		write(1, "%", 1);
	}
	return (count);
}

int	ft_printf(const char *str, ...)
{
	va_list	args;
	int		i;
	int		count;

	if (!str)
		return (-1);
	i = 0;
	count = 0;
	va_start(args, str);
	while (str[i])
	{
		if (str[i] == '%')
		{
			count += ft_condition(str[i + 1], args);
			i++;
		}
		else
		{
			count++;
			write(1, &str[i], 1);
		}
		i++;
	}
	va_end(args);
	return (count);
}

// #include <stdio.h>
// int	main()
// {
// 	//printf("NULL %s NULL", NULL);
	
// 	char a = 'c';
// 	char *b = "bonjour";
// 	//char *c = "bob";
// 	int d = -2147483648;
// 	//int d = 2147483647;
// 	char r = '\0';

// 	int	longueur;
// 	int	longueur2;
	
// 	printf("%d | ", ft_printf("qjd%sdqd",b));
// 	printf("%d\n\n", printf("qjd%sdqd",b));
	
// 	printf("%d | ", ft_printf(NULL));
// 	printf("%d\n\n", printf(NULL));
	
// 	printf("%d | ", ft_printf(""));
// 	printf("%d\n\n", printf(""));
	
// 	printf("%d | ", ft_printf("%c", a));
// 	printf("%d\n\n", printf("%c", a));
	
// 	printf("%d | ", ft_printf("%s", b));
// 	printf("%d\n\n", printf("%s", b));
	
// 	printf("%d | ", ft_printf("%p", (void *)d));
// 	printf("%d\n\n", printf("%p", d));
	
// 	printf("%d | ", ft_printf("%d", d));
// 	printf("%d\n\n", printf("%d", d));
	
// 	printf("%d | ", ft_printf("%i", d));
// 	printf("%d\n\n", printf("%i", d));
	
// 	printf("%d | ", ft_printf("%u", d));
// 	printf("%d\n\n", printf("%u", d));
	
// 	printf("%d | ", ft_printf("%x", d));
// 	printf("%d\n\n", printf("%x", d));
	
// 	printf("%d | ", ft_printf("%X", d));
// 	printf("%d\n\n", printf("%X", d));
	
// 	printf("%d | ", ft_printf("%%"));
// 	printf("%d\n\n", printf("%%"));
// 	longueur2 = ft_printf("%c %s %p %d %i %u %x %X %%\n", a, b, r, d, d, d, d, d);
// 	longueur =     printf("%c %s %p %d %i %u %x %X %%\n", a, b, r, d, d, d, d, d);

// 	printf("\n\nOriginal : %d\nMein : %d", longueur, longueur2);
// }
