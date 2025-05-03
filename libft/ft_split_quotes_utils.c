/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split_quotes_utils.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrollon- <jrollon-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 17:06:36 by jrollon-          #+#    #+#             */
/*   Updated: 2025/05/03 17:08:24 by jrollon-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "../inc/minishell_j.h"

int	is_escaped(t_split *squotes, size_t *i)
{
	size_t	j;

	j = *i;
	if (*i)
	{
		while (j > 0 && squotes->s[j - 1] == '\\')
			j--;
		if (((*i) - j) % 2 != 0)
			return (1);	
	}
	return (0);
}
