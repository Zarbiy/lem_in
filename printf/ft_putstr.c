/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_puststr_int.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cornguye <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/20 12:53:27 by cornguye          #+#    #+#             */
/*   Updated: 2023/11/20 12:53:52 by cornguye         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "ft_printf.h"

int	ft_putstr(char *str)
{
	int	i;

	if (str == NULL)
	{
		write (1, "(null)", 6);
		return (6);
	}
	i = 0;
	if (!str)
		return (0);
	while (str[i])
	{
		write (1, &str[i], 1);
		i++;
	}
	return (i);
}
