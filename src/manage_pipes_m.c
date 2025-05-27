/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   manage_pipes_m.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mpico-bu <mpico-bu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 11:32:40 by mpico-bu          #+#    #+#             */
/*   Updated: 2025/05/27 14:26:22 by mpico-bu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell_m.h"
#include "../inc/minishell_j.h"

int	count_pipes(t_input *input)
{
	int	i;
	int	count;

	i = 0;
	count = 0;
	while (input->split_exp[i])
	{
		if (ft_strcmp(input->split_exp[i], "|") == 0 && input->status_exp[i] == 0)
			count++;
		i++;
	}
	return (count);
}

void execute_pipeline(t_input *input)
{
	int num_cmds = count_pipes(input) + 1;
	int prev_fd = -1;
	int cmd_start = 0;

	for (int cmd = 0; cmd < num_cmds; cmd++)
	{
		int pipefd[2];
		int i, cmd_end = cmd_start;

		// Localiza el fin de este comando
		while (input->split_exp[cmd_end] &&
			!(ft_strcmp(input->split_exp[cmd_end], "|") == 0 && input->status_exp[cmd_end] == 0))
			cmd_end++;

		// Construye args
		char **args = malloc(sizeof(char *) * (cmd_end - cmd_start + 1));
		for (i = cmd_start; i < cmd_end; i++)
			args[i - cmd_start] = input->split_exp[i];
		args[i - cmd_start] = NULL;

		// Solo creamos una pipe si no es el último comando
		if (cmd < num_cmds - 1)
		{
			if (pipe(pipefd) == -1)
			{
				perror("pipe");
				exit(EXIT_FAILURE);
			}
		}

		pid_t pid = fork();
		if (pid == -1)
		{
			perror("fork");
			exit(EXIT_FAILURE);
		}

		if (pid == 0) // Hijo
		{
			// Si hay input desde pipe previa
			if (prev_fd != -1)
			{
				dup2(prev_fd, 0);
				close(prev_fd);
			}
			// Si hay output hacia próxima pipe
			if (cmd < num_cmds - 1)
			{
				close(pipefd[0]);
				dup2(pipefd[1], 1);
				close(pipefd[1]);
			}
			execvp(args[0], args);
			perror("execvp failed");
			exit(EXIT_FAILURE);
		}
		else // Padre
		{
			if (prev_fd != -1)
				close(prev_fd);
			if (cmd < num_cmds - 1)
			{
				close(pipefd[1]); // No escribir desde padre
				prev_fd = pipefd[0]; // Este será el input del próximo hijo
			}
			free(args); // Liberamos el array (pero no sus contenidos, pues los apuntan a split_exp)
		}
		cmd_start = cmd_end + 1; // Siguiente comando después de '|'
	}

	// Espera a todos
	for (int j = 0; j < num_cmds; j++)
		wait(NULL);
}


void ft_manage_pipes(t_input *input)
{
	if (count_pipes(input) == 0)
		ft_manage_input(input);  // Ya lo tenías definido
	else
		execute_pipeline(input);
	if (input)
		ft_input_free(input);
}