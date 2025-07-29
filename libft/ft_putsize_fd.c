/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putsize_fd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ltheveni <ltheveni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 14:24:26 by ltheveni          #+#    #+#             */
/*   Updated: 2025/05/03 14:25:55 by ltheveni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_putsize_fd(size_t n, int fd)
{
	char	result;

	if (n > 9)
		ft_putsize_fd(n / 10, fd);
	result = (n % 10) + '0';
	write(fd, &result, 1);
}
