/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bi_export_m.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mpico-bu <mpico-bu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 19:06:31 by mpico-bu          #+#    #+#             */
/*   Updated: 2025/05/23 17:26:33 by mpico-bu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell_m.h"
#include "../inc/minishell_j.h"
static void	sort_env(char **env, int size)
{
	int		i;
	int		j;
	char	*tmp;

	i = 0;
	while (i < size - 1)
	{
		j = i + 1;
		while (j < size)
		{
			if (ft_strcmp(env[i], env[j]) > 0)
			{
				tmp = env[i];
				env[i] = env[j];
				env[j] = tmp;
			}
			j++;
		}
		i++;
	}
}

void	print_sorted_env(char **envp)
{
	char	**sorted;
	int		count = 0;
	int		i = 0;
	char	*equal;

	while (envp[count])
		count++;
	sorted = ft_calloc(sizeof(char *), count + 1);
	if (!sorted)
		return ;
	while (i < count)
	{
		sorted[i] = ft_strdup(envp[i]);
		i++;
	}
	sort_env(sorted, count);
	i = 0;
	while (i < count)
	{
		equal = ft_strchr(sorted[i], '=');
		ft_printf("declare -x ");
		if (equal)
		{
			write(1, sorted[i], equal - sorted[i]);
			ft_printf("=\"%s\"\n", equal + 1);
		}
		else
			ft_printf("%s\n", sorted[i]);
		free(sorted[i]);
		i++;
	}
	free(sorted);
}

// ✅ Solo une si el valor está entrecomillado
char	*join_quoted_value(t_input *input, int start)
{
	char	*joined = NULL;
	int		i = start;

	if (!is_quoted(input, i))
		return (ft_strdup(input->input_split[i])); // NO unir si no está entrecomillado

	while (input->input_split[i])
	{
		if (!joined)
			joined = ft_strdup(input->input_split[i]);
		else
		{
			char *tmp = joined;
			joined = ft_strjoin(tmp, " ");
			free(tmp);
			tmp = joined;
			joined = ft_strjoin(joined, input->input_split[i]);
			free(tmp);
		}
		if (is_quoted(input, i))
			break;
		i++;
	}
	return (joined);
}

bool	ft_manage_shlvl(char *input, char **envp, int i, int len)
{
	int	j = len + 1;

	free(envp[i]);
	while (input[j])
	{
		if (ft_isdigit(input[j++]))
			continue;
		envp[i] = ft_strdup("SHLVL=0");
		return (1);
	}
	envp[i] = ft_strdup(input);
	return (1);
}

bool	ft_is_valid_identifier(const char *str)
{
	int	i;

	if (!str || !str[0])
		return (false);
	if (!ft_isalpha(str[0]) && str[0] != '_')
		return (false);
	i = 1;
	while (str[i] && str[i] != '=')
	{
		if (!ft_isalnum(str[i]) && str[i] != '_')
			return (false);
		i++;
	}
	return (true);
}

bool	ft_check_variables(char *input, char **envp)
{
	int		i;
	int		len;
	char	*equal;

	if (!ft_is_valid_identifier(input))
		return (1);
	equal = ft_strchr(input, '=');
	if (!equal)
		return (0); // Solo un nombre, sin valor

	len = equal - input;
	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], input, len) == 0 && envp[i][len] == '=')
		{
			if (ft_strncmp("SHLVL", input, len) == 0)
				return (ft_manage_shlvl(input, envp, i, len), 1);
			free(envp[i]);
			envp[i] = ft_strdup(input);
			return (!envp[i]); // return 1 on success
		}
		i++;
	}
	return (0);
}

void	ft_add_to_env(char *new_var, char ***envp)
{
	int		i = 0;
	char	**new_env;

	while ((*envp)[i])
		i++;
	new_env = ft_calloc(i + 2, sizeof(char *));
	if (!new_env)
		return ;
	i = 0;
	while ((*envp)[i])
	{
		new_env[i] = ft_strdup((*envp)[i]);
		if (!new_env[i])
		{
			ft_matrix_free(&new_env);
			return ;
		}
		i++;
	}
	new_env[i] = ft_strdup(new_var);
	if (!new_env[i])
	{
		ft_matrix_free(&new_env);
		return ;
	}
	ft_matrix_free(envp);
	*envp = new_env;
}

void	ft_export(t_input *input, char ***envp)
{
	int		i = 1;
	bool	error = false;

	if (!input->input_split[1])
	{
		print_sorted_env(*envp);
		return ;
	}
	while (input->input_split[i])
	{
		char *arg = join_quoted_value(input, i);
		if (!arg)
			break;

		// Avanza el índice al final de la unión
		int consumed = 1;
		if (ft_strchr(arg, '"'))
		{
			while (input->input_split[i + consumed] &&
				!is_quoted(input, i + consumed))
				consumed++;
			consumed++; // Para incluir el último token con la comilla de cierre
		}

		char *equal = ft_strchr(arg, '=');
		char *name = (equal) ? ft_substr(arg, 0, equal - arg) : ft_strdup(arg);

		if (!ft_is_valid_identifier(name))
		{
			error = true;
			ft_putstr_fd("miniyo: export: not a valid identifier\n", 2);
			free(name);
			free(arg);
			i += consumed;
			continue;
		}
		free(name);

		if (ft_check_variables(arg, *envp) == 0)
			ft_add_to_env(arg, envp);

		free(arg);
		i += consumed;
	}

	input->last_exit_code = error ? 1 : 0;
}





/*1. checks if the variable already exists in envp if so, exit*/
/*2. if not create a new **new_env VAR with space for the new VAR plus the...*/
/*...the final NULL*/

/* 
void	ft_export(t_input *input, char ***envp)
{
	int		i;
	char	*arg;

	if (!input->input_split[1]) // solo `export` → imprimir
	{
		print_sorted_env(*envp);
		input->last_exit_code = 0;
		return ;
	}

	i = 1;
	while (input->input_split[i])
	{
		arg = input->input_split[i];
		if (ft_check_variables(arg, *envp))
			input->last_exit_code = 1;
		else
		{
			if (ft_strchr(arg, '='))
			{
				ft_add_to_env(arg, envp);
			}
		}
		i++;
	}
} */
