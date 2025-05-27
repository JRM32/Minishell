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
	int i = 0, cmd_start = 0;
	int num_cmds = count_pipes(input) + 1;
	int pipefd[2], prev_fd = -1;
	pid_t pid;

	for (int cmd = 0; cmd < num_cmds; cmd++)
	{
		// Encuentra el final del comando actual
		int cmd_end = cmd_start;
		while (input->split_exp[cmd_end] &&
			   !(ft_strcmp(input->split_exp[cmd_end], "|") == 0 && input->status_exp[cmd_end] == 0))
			cmd_end++;

		// Extrae los argumentos del comando actual
		char **args = malloc(sizeof(char *) * (cmd_end - cmd_start + 1));
		for (i = cmd_start; i < cmd_end; i++)
			args[i - cmd_start] = input->split_exp[i];
		args[i - cmd_start] = NULL;

		// Prepara pipe si no es el último comando
		if (cmd < num_cmds - 1)
			pipe(pipefd);

		pid = fork();
		if (pid == 0) // Proceso hijo
		{
			if (prev_fd != -1)
			{
				dup2(prev_fd, 0); // stdin desde anterior pipe
				close(prev_fd);
			}
			if (cmd < num_cmds - 1)
			{
				close(pipefd[0]);      // Cierra read
				dup2(pipefd[1], 1);    // stdout al write de la pipe
				close(pipefd[1]);
			}
			execvp(args[0], args);
			perror("execvp failed");
			exit(EXIT_FAILURE);
		}
		else if (pid > 0)
		{
			// Proceso padre
			if (prev_fd != -1)
				close(prev_fd);
			if (cmd < num_cmds - 1)
			{
				close(pipefd[1]);  // Cerramos write
				prev_fd = pipefd[0]; // Guardamos read
			}
			free(args);
		}
		cmd_start = cmd_end + 1; // Salta la pipe
	}

	// Espera a todos los procesos
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