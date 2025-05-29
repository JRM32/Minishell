/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_command_m.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mpico-bu <mpico-bu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 12:49:50 by mpico-bu          #+#    #+#             */
/*   Updated: 2025/05/29 17:14:02 by mpico-bu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell_m.h"
#include "../inc/minishell_j.h"

void	update_exit_code_env(t_input *input, int exit_code)
{
	char	*new_var;
	char	**envp;
	int		i;

	char *code_str = ft_itoa(exit_code);
	if (!code_str)
		return ;
	new_var = ft_strjoin("?", "=");
	if (!new_var)
	{
		free(code_str);
		return ;
	}
	char *tmp = ft_strjoin(new_var, code_str);
	free(new_var);
	free(code_str);
	if (!tmp)
		return ;
	new_var = tmp;

	envp = input->envp;
	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], "?", 1) == 0 && envp[i][1] == '=')
		{
			free(envp[i]);
			envp[i] = new_var;
			input->last_exit_code = exit_code;
			return ;
		}
		i++;
	}
	int len = 0;
	while (envp[len])
		len++;
	char **new_envp = (char **)malloc(sizeof(char *) * (len + 2));
	if (!new_envp)
	{
		free(new_var);
		return;
	}
	i = 0;
	while (i < len)
	{
		new_envp[i] = envp[i];
		i++;
	}
	new_envp[i] = new_var;
	new_envp[i + 1] = NULL;
	free(input->envp);
	input->envp = new_envp;
	input->last_exit_code = exit_code;
}


// Devuelve el valor de la variable de entorno PATH.
char	*get_path_env(char **envp)
{
	int	i;

	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], "PATH=", 5) == 0)
			return (&envp[i][5]);
		i++;
	}
	return (NULL);
}

// Concatena un directorio y comando en una ruta completa.
char	*join_path(const char *dir, const char *command)
{
	char	*full_path;
	size_t	len;

	len = ft_strlen(dir) + ft_strlen(command) + 2;
	full_path = ft_calloc(1, len * sizeof(char));
	if (!full_path)
		return (NULL);
	ft_strlcpy(full_path, dir, len);
	ft_strlcat(full_path, "/", len);
	ft_strlcat(full_path, command, len);
	return (full_path);
}

// Busca un ejecutable en los directorios de PATH.

char *find_executable(char *command, char **envp)
{
	char *path, **dirs, *full_path;
	int i;
	struct stat sb;

	if (!command || command[0] == '\0')
		return (NULL);

	if (ft_strchr(command, '/'))
	{
		if (stat(command, &sb) == 0)
		{
			if (S_ISDIR(sb.st_mode))
			{
				errno = EISDIR;
				return (NULL);
			}
			if (access(command, X_OK) != 0)
			{
				errno = EACCES;
				return (NULL);
			}
			return ft_strdup(command);
		}
		return NULL;
	}

	path = get_path_env(envp);
	if (!path)
		return NULL;

	dirs = ft_split(path, ':');
	if (!dirs)
		return NULL;
	i = 0;
	while (dirs[i])
	{
		full_path = join_path(dirs[i], command);
		if (!full_path)
			continue;

		if (stat(full_path, &sb) == 0)
		{
			if (S_ISDIR(sb.st_mode))
			{
				errno = EISDIR;
				ft_matrix_free(&dirs);
				free(full_path);
				return NULL;
			}
			if (access(full_path, X_OK) == 0)
			{
				ft_matrix_free(&dirs);
				return full_path;
			}
			else
			{
				errno = EACCES;
				ft_matrix_free(&dirs);
				free(full_path);
				return NULL;
			}
		}
		free(full_path);
		i++;
	}
	ft_matrix_free(&dirs);
	return NULL;
}

bool exec_child(t_input *input, pid_t pid, char *executable)
{
    char **command_union;
    char **args_split = NULL;
    int argc = 0;

    // Split input->args if it's non-empty
    if (input->args && input->args[0])
    {
        args_split = ft_split(input->args, ' ');
        if (!args_split)
            return false;

        while (args_split[argc])
            argc++;
    }

    // Allocate command_union (command + args + NULL)
    command_union = (char **)ft_calloc(argc + 2, sizeof(char *));
    if (!command_union)
    {
        if (args_split)
            ft_matrix_free(&args_split);
        return false;
    }

    // Build the argv array for execve
    command_union[0] = input->command;
    for (int i = 0; i < argc; i++)
        command_union[i + 1] = args_split[i];
    command_union[argc + 1] = NULL;

    if (pid == -1)
    {
        perror("fork");
        free(executable);
        free(command_union);  // only free the outer array, args_split memory is reused
        return false;
    }

    if (pid == 0)
    {
        if (input->inputfd != STDIN_FILENO)
            dup2(input->inputfd, STDIN_FILENO);
        if (input->outputfd != STDOUT_FILENO)
            dup2(input->outputfd, STDOUT_FILENO);

        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);

        execve(executable, command_union, input->envp);

        // If execve fails
        perror("execve");
        free(command_union);
        free(executable);
        exit(1);
    }

    // Only parent continues here
    free(command_union);  // Don't free args_split strings—they're part of command_union now
    return true;
}
bool execute_command(t_input *input)
{
	char *executable;
	pid_t pid;
	int status, sig;

	ft_printf("command:%s\n", input->command);
	ft_printf("arg:%s\n", input->args);
	ft_printf("parsed:%s\n", input->parsed);
	for (size_t i = 0; input->input_split[i]; i++)
		ft_printf("Input split%d:%s %d\n", i, input->input_split[i], input->status_exp[i]);
	for (size_t i = 0; input->split_exp[i]; i++)
		ft_printf("Split exp%d:%s %d\n", i, input->split_exp[i], input->status_exp[i]);
	ft_printf("inputfd:%d\n", input->inputfd);
	ft_printf("outputfd:%d\n", input->outputfd);
	ft_printf("-----SALIDA-----\n\n\n\n"); 

	if (!input->input_split || !input->input_split[0] || !input->command || input->command[0] == '\0')
	{
		update_exit_code_env(input, 0);
		return true;
	}

	errno = 0;
	executable = find_executable(input->command, input->envp);
	
	if (!executable)
	{
		if (errno == EACCES)
		{
			ft_putstr_fd("miniyo: Permission denied\n", 2);
			update_exit_code_env(input, 126);
		}
		else if (errno == EISDIR)
		{
			ft_putstr_fd("miniyo: Is a directory\n", 2);
			update_exit_code_env(input, 126);
		}
		else
		{
			ft_putstr_fd("miniyo: command not found\n", 2);
			update_exit_code_env(input, 127);
		}
		return false;
	}
	

	pid = fork();
	
	if (!exec_child(input, pid, executable))
	{
		free(executable);
		return false;
	}
	signal(SIGINT, SIG_IGN);
	waitpid(pid, &status, 0);


	if (WIFSIGNALED(status))
	{
		sig = WTERMSIG(status);
		if (sig == SIGINT)
			write(1, "\n", 1);
		else if (sig == SIGQUIT)
			write(1, "Quit (core dumped)\n", 19);
		update_exit_code_env(input, 128 + sig);
	}
	else if (WIFEXITED(status))
		update_exit_code_env(input, WEXITSTATUS(status));
	else
		update_exit_code_env(input, 1);

	signal(SIGINT, ctrlc_handler);

	free(executable);
	return input->last_exit_code == 0;
}