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

bool	ft_manage_redirection(t_input *input, char *search, int i, bool lonely)
{
	if (ft_strcmp(search, "<") == 0)
		if (ft_manage_input_redirection(input, i, lonely) == 0)
			return (0);
	if (ft_strcmp(search, ">") == 0)
		if (ft_manage_output_redirection(input, i, lonely) == 0)
			return (0);
	if (ft_strcmp(search, ">>") == 0)
		if (ft_manage_append_redirection(input, i, lonely) == 0)
			return (0);
	if (ft_strcmp(search, "<<") == 0)
		if (ft_manage_heredoc_redirection(input, i, lonely) == 0)
			return (0);
	return (1);
}

int	ft_check_one_redirection(t_input *input, int i, char *search)
{
	bool	lonely;

	lonely = false;
	if (ft_strncmp(input->split_exp[i], search, ft_strlen(search)) == 0)
	{
		if (ft_strcmp(input->split_exp[i], search) == 0)
			lonely = true;
		if (ft_manage_redirection(input, search, i, lonely) == 0)
			return (-1);
		return (1);
	}
	return (0);
}

char	**ft_create_redirection(void)
{
	char	**redirections;

	redirections = malloc(sizeof(char *) * 5);
	redirections[0] = ft_strdup("<<");
	redirections[1] = ft_strdup("<");
	redirections[2] = ft_strdup(">>");
	redirections[3] = ft_strdup(">");
	redirections[4] = NULL;
	return (redirections);
}

bool	handle_redirection(t_input *input)
{
	int		i;
	int		j;
	int		result;
	char	**redirections;

	redirections = ft_create_redirection();
	i = -1;
	while (input->split_exp[++i])
	{
		j = -1;
		while (++j < 4)
		{
			if (input->status_exp[i] != 0)
				continue ;
			result = ft_check_one_redirection(input, i, redirections[j]);
			if (result == 1)
			{
				i = -1;
				break ;
			}
			else if (result == -1)
				return (ft_matrix_free(&redirections), 1);
		}
	}
	return (ft_matrix_free(&redirections), 0);
}
	/*
	printf("ANTES\n");
	for (int j = 0; input->split_exp[j]; j++)
		printf("Split exp %d: %s\n", j, input->split_exp[j]);
	printf("input: %s\n", input->input);
	printf("parsed: %s\n", input->parsed);
	printf("command: %s\n", input->command);
	printf("args: %s\n", input->args);

	//printf("inputfd: %d\n", input->inputfd);
	//printf("outputfd: %d\n", input->outputfd);
	printf("________\n");
	*/