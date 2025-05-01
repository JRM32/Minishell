/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_command_m.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mpico-bu <mpico-bu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 12:49:50 by mpico-bu          #+#    #+#             */
/*   Updated: 2025/05/02 00:59:01 by mpico-bu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell_m.h"
#include "../inc/minishell_j.h"

// Devuelve el valor de la variable de entorno PATH.
char	*get_path_env(char **envp)
{
	int	i;

	i = 0;
	while (envp[i])
	{
		if (ft_strnstr(envp[i], "PATH=", 5))
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
char	*find_executable(char *command, char **envp)
{
	char	*path;
	char	**dirs;
	char	*full_path;
	int		i;

	if (access(command, X_OK) == 0)
		return (ft_strdup(command));
	path = get_path_env(envp);
	if (!path)
		return (NULL);
	dirs = ft_split(path, ':');
	if (!dirs)
		return (NULL);
	i = 0;
	while (dirs[i])
	{
		full_path = join_path(dirs[i++], command);
		if (full_path && access(full_path, X_OK) == 0)
			return (ft_matrix_free(dirs), full_path);
		free(full_path);
	}
	ft_matrix_free(dirs);
	return (NULL);
}

// Divide un comando en sus argumentos.
char	**split_command(const char *command)
{
	char	**args;

	args = ft_split(command, ' ');
	if (!args)
	{
		perror("malloc");
		exit(1);
	}
	return (args);
}

// Ejecuta un comando en un proceso hijo con redirección.
void	execute_command(char *cmd, int input_fd, int output_fd, char **envp)
{
	char	**args;
	char	*executable;

	if (input_fd != STDIN_FILENO)
		dup2(input_fd, STDIN_FILENO);
	if (output_fd != STDOUT_FILENO)
		dup2(output_fd, STDOUT_FILENO);
	args = split_command(cmd);
	if (!args || !args[0])
		exit(1);
	executable = find_executable(args[0], envp);
	if (!executable)
	{
		write(STDERR_FILENO, "command not found\n", 18);
		ft_matrix_free(args);
		exit(127);
	}
	execve(executable, args, envp);
	perror("execve");
	free(executable);
	ft_matrix_free(args);
	exit(127);
}
