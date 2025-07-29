/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putsize_base_fd.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ltheveni <ltheveni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 14:27:40 by ltheveni          #+#    #+#             */
/*   Updated: 2025/05/03 16:01:05 by ltheveni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_putsize_base_fd(size_t n, char *base, int fd, int padding)
{
	size_t	base_len;
	char	buffer[65];
	int		i;

	base_len = 0;
	i = 0;
	while (base[base_len])
		base_len++;
	if (n == 0)
		buffer[i++] = base[0];
	else
	{
		while (n > 0)
		{
			buffer[i++] = base[n % base_len];
			n /= base_len;
		}
	}
	while (i < padding)
		buffer[i++] = '0';
	while (--i >= 0)
		write(fd, &buffer[i], 1);
}
