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

char *join_command(char **split_exp, int start, int end) {
    int i;
    size_t len = 0;
    char *joined;

    // Calculate total length
    for (i = start; i < end; i++) {
        len += strlen(split_exp[i]) + 1;
    }

    joined = malloc(len + 1);
    if (!joined)
        return NULL;
    
    joined[0] = '\0';
    for (i = start; i < end; i++) {
        strcat(joined, split_exp[i]);
        if (i < end - 1)
            strcat(joined, " ");
    }

    return joined;
}


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
	pid_t last_pid = -1;

	for (int cmd = 0; cmd < num_cmds; cmd++)
	{
		int pipefd[2];
		int i, cmd_end = cmd_start;

		// Localiza el fin de este comando
		while (input->split_exp[cmd_end] &&
			!(ft_strcmp(input->split_exp[cmd_end], "|") == 0 && input->status_exp[cmd_end] == 0))
			cmd_end++;

		// Construye args
		// Build input_child
		t_input input_child;
		ft_bzero(&input_child, sizeof(t_input)); // Make sure it's zeroed

		// Generate input string
		input_child.input = join_command(input->split_exp, cmd_start, cmd_end);
		input_child.input_split = ft_split_quotes(input_child.input, ' ', &input_child);
		compose_command_args(&input_child);
		parsing(&input_child);

		// Debug print
		printf("Process %d input_child.input: %s\n", cmd, input_child.input);
		
		 printf("============\nPARSEADO:%s\n==========\n", input_child.parsed);
		printf("command:%s\n", input_child.command);
		printf("arg:%s\n", input_child.args);
		for (size_t i = 0; input_child.split_exp[i]; i++)//
		ft_printf("%d.%s %d\n", i, input_child.split_exp[i], input_child.status_exp[i]);//
			
		ft_input_free(&input_child); // Free input_child if needed

		// Use existing logic to re-parse this string into input_child.split_exp etc.
		// This depends on how you handle parsing normally
		//ft_parse_input(&input_child); // Hypothetical function

		// Build args from input_child (or just reuse old logic if already correct)
		int argc = cmd_end - cmd_start;
		char **args = malloc(sizeof(char *) * (argc + 1));
		for (i = 0; i < argc; i++)
			args[i] = input->split_exp[cmd_start + i];
		args[i] = NULL;

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
			if (prev_fd != -1)
			{
				dup2(prev_fd, 0);
				close(prev_fd);
			}
			if (cmd < num_cmds - 1)
			{
				close(pipefd[0]);
				dup2(pipefd[1], 1);
				close(pipefd[1]);
			}
			execvp(args[0], args);
			exit(EXIT_FAILURE);
		}
		else // Padre
		{
			if (prev_fd != -1)
				close(prev_fd);
			if (cmd < num_cmds - 1)
			{
				close(pipefd[1]);
				prev_fd = pipefd[0];
			}
			free(args);
			last_pid = pid; // Guardamos el último PID
		}

		cmd_start = cmd_end + 1;
	}

	// Espera a todos y guarda el exit code del último hijo
	int status;
	pid_t wpid;
	while ((wpid = wait(&status)) > 0)
	{
		if (wpid == last_pid && WIFEXITED(status))
			input->last_exit_code = WEXITSTATUS(status);
	}
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