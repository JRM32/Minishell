/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bi_exit_j.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrollon- <jrollon-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 11:16:14 by jrollon-          #+#    #+#             */
/*   Updated: 2025/05/18 19:25:10 by jrollon-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell_m.h"
#include "../inc/minishell_j.h"

void	ft_exit(t_input *in)
{
	long long	arg;
	int			error;
	
	error = 0;
	arg = ft_atol(in->args, &error);
	
	
	//clean_all(in, 0);
	//rl_clear_history();
	//exit(0);
	printf("error:%d\n", error);
	printf("exit:%lld\n", arg);
}
