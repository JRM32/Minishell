/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrollon- <jrollon-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 17:53:26 by mpico-bu          #+#    #+#             */
/*   Updated: 2025/05/03 17:37:23 by jrollon-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell_m.h"
#include "../inc/minishell_j.h"

void	ft_manage_input(t_input *input)
{
	input->input_split = ft_split_quotes(input->input, ' ', input);
	if (!input->input_split || !input->input_split[0])
		return ;
	if (ft_strcmp(input->input_split[0], "pwd") == 0)
		ft_pwd(input->input_split);
	else if (ft_strcmp(input->input_split[0], "cd") == 0)
		ft_cd(input->input_split, input->envp);
	else if (ft_strcmp(input->input_split[0], "echo") == 0)
		ft_echo(input->input_split, input);
	else if (ft_strcmp(input->input_split[0], "export") == 0
		&& input->input_split[1])
		ft_export(input->input_split[1], &input->envp);
	else if (ft_strcmp(input->input_split[0], "env") == 0)
		ft_env(input->input_split, input->envp);
	else if (ft_strcmp(input->input_split[0], "unset") == 0
		&& input->input_split[1])
		ft_unset(input->input_split[1], &input->envp);
	else if (execute_command(input->input_split, 0, 0, input->envp) == 1)
		return (ft_input_free(input));
	else
		printf("%s: command not found\n", input->input_split[0]);
	ft_input_free(input);
}

void	ft_manage_pipes(t_input *input)
{
	char	**cmds;
	int		pipefd[2];
	pid_t	pid1;
	pid_t	pid2;
	int		status;

	cmds = ft_split(input->input, '|');
	if (!cmds || !cmds[0] || !cmds[1])
		return ;
	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		ft_matrix_free(cmds);
		return ;
	}
	pid1 = fork();
	if (pid1 == -1)
	{
		perror("fork");
		ft_matrix_free(cmds);
		return ;
	}
	if (pid1 == 0)
	{
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		execute_command(&cmds[0], 0, STDOUT_FILENO, input->envp);
		exit(1);
	}
	pid2 = fork();
	if (pid2 == -1)
	{
		perror("fork");
		ft_matrix_free(cmds);
		return ;
	}
	if (pid2 == 0)
	{
		close(pipefd[1]);
		dup2(pipefd[0], STDIN_FILENO);
		close(pipefd[0]);
		execute_command(&cmds[1], STDIN_FILENO, 1, input->envp);
		exit(1);
	}
	close(pipefd[0]);
	close(pipefd[1]);
	waitpid(pid1, &status, 0);
	waitpid(pid2, &status, 0);
	ft_matrix_free(cmds);
}

int	main(int argc, char **argv, char **envp)
{
	t_input				input;
	struct sigaction	sa;

	(void)argc;
	(void)argv;
	input.envp = ft_matrix_dup(envp);
	if (!input.envp)
		return (clean_all(&input), 1);
	init_sigaction(&sa);
	while (1)
	{
		input.input = readline("minishell$ ");
		if (!input.input)
			break ;
		if (input.input && *(input.input))
			ft_manage_history(input.input, 0);
		ft_manage_input(&input);
		free(input.input);
	}
	clean_all(&input);
	return (0);
}
