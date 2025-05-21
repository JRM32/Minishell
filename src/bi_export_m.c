/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bi_export_m.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mpico-bu <mpico-bu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 19:06:31 by mpico-bu          #+#    #+#             */
/*   Updated: 2025/05/22 01:24:46 by mpico-bu         ###   ########.fr       */
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
	int		count;
	int		i;
	char	*equal;

	count = 0;
	while (envp[count])
		count++;
	sorted = ft_calloc(sizeof(char *), count + 1);
	if (!sorted)
		return ;
	i = 0;
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

// Devuelve una nueva cadena con las palabras combinadas correctamente si están entrecomilladas
char	*join_quoted_value(t_input *input, int start)
{
	char	*joined = NULL;
	int		i = start;

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
		// si la palabra estaba cerrando comillas, rompemos
		if (is_quoted(input, i))
			break ;
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
			continue ;
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
	{
		ft_printf("minishell: export: `%s': not a valid identifier\n", input);
		return (1);
	}
	equal = ft_strchr(input, '=');
	if (!equal)
		return (1);
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
			if (!envp[i])
				return (0);
			return (1);
		}
		i++;
	}
	return (0);
}

void	ft_export(t_input *input_data, char ***envp)
{
	int		i;
	int		env_position;
	char	*prepared_input;
	char	**new_env;

	i = 1;
	if (!input_data->input_split[1])
	{
		print_sorted_env(*envp);
		return ;
	}
	while (input_data->input_split[i])
	{
		prepared_input = ft_strdup(input_data->input_split[i]);
		if (!ft_is_valid_identifier(prepared_input))
		{
			ft_printf("minishell: export: `%s': not a valid identifier\n", prepared_input);
			free(prepared_input);
			i++;
			continue ;
		}
		if (ft_check_variables(prepared_input, *envp) == 1)
		{
			free(prepared_input);
			i++;
			continue ;
		}
		env_position = 0;
		while ((*envp)[env_position])
			env_position++;
		new_env = ft_calloc(env_position + 2, sizeof(char *));
		if (!new_env)
			return (free(prepared_input));
		for (int j = 0; j < env_position; j++)
			new_env[j] = ft_strdup((*envp)[j]);
		new_env[env_position] = ft_strdup(prepared_input);
		free(prepared_input);
		ft_matrix_free(envp);
		*envp = new_env;

		i++;
	}
}

