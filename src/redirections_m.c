/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections_m.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mpico-bu <mpico-bu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 01:26:18 by mpico-bu          #+#    #+#             */
/*   Updated: 2025/05/29 17:09:46 by mpico-bu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell_m.h"
#include "../inc/minishell_j.h"

void ft_manage_input_redirection(t_input *input, int i, bool lonely)
{
    char *filename;

    if (lonely)
        filename = input->split_exp[i + 1];
    else
        filename = input->split_exp[i] + ft_strlen("<");
    if (input->inputfd > 2)
        close(input->inputfd);

    input->inputfd = open(filename, O_RDONLY);
    if (input->inputfd == -1)
		ft_putstr_fd("Error opening input file\n", 2);
	update_input(input, i, lonely);
}

void ft_manage_redirection(t_input *input, char *search, int i, bool lonely)
{
	if (ft_strcmp(search, "<") == 0)
		ft_manage_input_redirection(input, i, lonely);
	//if (ft_strcmp(search, ">") == 0)
	//	ft_manage_output_redirection(input, i, lonely);
	//if (ft_strcmp(search, "<<") == 0)
	//	ft_manage_heardoc_redirection(input, i, lonely);
	//if (ft_strcmp(search, ">>") == 0)
	//	ft_manage_append_redirection(input, i, lonely);
}

bool ft_check_one_redirection(t_input *input, int i, char *search)
{
	if (ft_strncmp(input->split_exp[i], search, ft_strlen(search)) == 0)
	{	
		if (ft_strcmp(input->split_exp[i], search) == 0)
			ft_manage_redirection(input, search, i, true);
		else
			ft_manage_redirection(input, search, i, false);
		free(search);
		return (1);
	}
	free(search);
	return (0);
}

bool handle_redirection(t_input *input)
{
	int i;

	i = 0;
	while (input->split_exp[i])
	{
		if (input->status_exp[i] != 0);
		else if (ft_check_one_redirection(input, i, ft_strdup("<<")) == 1)
			i = -1;
		else if (ft_check_one_redirection(input, i, ft_strdup("<")) == 1)
			i = -1;
		else if (ft_check_one_redirection(input, i, ft_strdup(">>")) == 1)
			i = -1;
		else if (ft_check_one_redirection(input, i, ft_strdup(">")) == 1)
			i = -1;
		i++;
	}
	//for (int j = 0; input->split_exp[j]; j++)
	//	printf("Split exp %d: %s\n", j, input->split_exp[j]);
	//printf("input: %s\n", input->input);
	//printf("command: %s\n", input->command);
	//printf("args: %s\n", input->args);
	//printf("inputfd: %d\n", input->inputfd);
	//printf("outputfd: %d\n", input->outputfd);
	return (0);
}
