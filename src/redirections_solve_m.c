/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections_m.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrollon- <jrollon-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 01:26:18 by mpico-bu          #+#    #+#             */
/*   Updated: 2025/05/30 10:45:46 by jrollon-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell_m.h"
#include "../inc/minishell_j.h"

bool	ft_manage_input_redirection(t_input *input, int i, bool lonely)
{
	char	*filename;

	if (lonely)
		filename = input->split_exp[i + 1];
	else
		filename = input->split_exp[i] + ft_strlen("<");
	if (input->inputfd > 2)
		close(input->inputfd);
	input->inputfd = open(filename, O_RDONLY);
	if (input->inputfd == -1)
	{
		input->last_exit_code = 1;
		ft_putstr_fd("miniyo: No such file or directory\n", 2);
		return (0);
	}
	update_input(input, i, lonely);
	return (1);
}

bool	ft_manage_output_redirection(t_input *input, int i, bool lonely)
{
	char	*filename;

	if (lonely)
		filename = input->split_exp[i + 1];
	else
		filename = input->split_exp[i] + ft_strlen(">");
	if (input->outputfd > 2)
		close(input->outputfd);
	input->outputfd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (input->outputfd == -1)
	{
		input->last_exit_code = 1;
		ft_putstr_fd("miniyo: Permission denied\n", 2);
		return (0);
	}
	update_input(input, i, lonely);
	return (1);
}

bool	ft_manage_append_redirection(t_input *input, int i, bool lonely)
{
	char	*filename;

	if (lonely)
		filename = input->split_exp[i + 1];
	else
		filename = input->split_exp[i] + ft_strlen(">>");
	if (input->outputfd > 2)
		close(input->outputfd);
	input->outputfd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (input->outputfd == -1)
	{
		input->last_exit_code = 1;
		ft_putstr_fd("miniyo: Permission denied\n", 2);
		return (0);
	}
	update_input(input, i, lonely);
	return (1);
}
