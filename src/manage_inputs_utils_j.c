/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   manage_inputs_utils_j.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrollon- <jrollon-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 09:31:56 by jrollon-          #+#    #+#             */
/*   Updated: 2025/05/06 09:32:34 by jrollon-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell_m.h"
#include "../inc/minishell_j.h"

void	compose_args(t_input *in, size_t word)
{
	size_t	i;
	size_t	j;
	size_t	k;
	int		exit_while;
	int		status_done;

	k = 0;
	i = word;
	exit_while = 0;
	status_done = 0;
	ft_bzero(in->args, 100);
	while (i < in->input_words)
	{
		j = 0;
		status_done = 0;
		if (i > word && in->input_split[i][0] == '\0'
			&& (in->status[i] == SQUO_SP || in->status[i] == DQUO_SP))
			break ;
		while ((in->input_split[i][j] != ' ' || is_quoted(in, i))
			&& in->input_split[i][j] != '\0' && !exit_while)
		{
			if ((i > word && (in->status[i] == EPTY_SP
					|| in->status[i] == SQUO_SP || in->status[i] == DQUO_SP)
					&& !status_done))
				exit_while = 1;
			else
				in->args[k++] = in->input_split[i][j++];
			status_done = 1;
		}
		if (in->input_split[i][j] == ' ' || exit_while)
			break ;
		i++;
	}
	in->word_after_arg = i;
}

void	compose_command_args(t_input *in)
{
	size_t	i;
	size_t	j;
	size_t	k;
	int		exit_while;
	int		status_done;

	i = 0;
	k = 0;
	exit_while = 0;
	status_done = 0;
	ft_bzero(in->command, 250);
	while (i < in->input_words)
	{
		j = 0;
		status_done = 0;
		if (i > 0 && in->input_split[i][0] == '\0' && (in->status[i] == SQUO_SP
			|| in->status[i] == DQUO_SP))
			break ;
		while ((in->input_split[i][j] != ' ' || is_quoted(in, i))
			&& (in->input_split[i][j] != '\0') && !exit_while)
		{
			if (i > 0 && (in->status[i] == EPTY_SP
					|| in->status[i] == SQUO_SP || in->status[i] == DQUO_SP)
				&& !status_done)
				exit_while = 1;
			else
				in->command[k++] = in->input_split[i][j++];
			status_done = 1;
		}
		if (in->input_split[i][j] == ' ' || exit_while)
			break ;
		i++;
	}
	in->word_after_command = i;
	compose_args(in, i);
}
