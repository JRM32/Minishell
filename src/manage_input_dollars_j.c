/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   manage_input_dollars_j.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrollon- <jrollon-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 12:59:27 by jrollon-          #+#    #+#             */
/*   Updated: 2025/05/17 14:10:49 by jrollon-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell_m.h"
#include "../inc/minishell_j.h"

int	save_valid_env_variable(t_input *n, size_t w, size_t *i, size_t *k)
{
	int		env_n;
	size_t	j;

	env_n = valid_env((n->input_split[w] + (*i) + 1), n, w);
	if (env_n > -1)
	{
		if (!(n->dollars % 2))
		{
			j = 0;
			while (n->envp[env_n][j] != '=')
				j++;
			j++;
			while (n->envp[env_n][j] == ' ')
				j++;
			while (n->envp[env_n][j])
			{
				n->command[(*k)++] = n->envp[env_n][j++];
				if (n->envp[env_n][j] == ' ')
					break ;
			}
		}
		else
		{
			while (n->input_split[w][*i] && n->input_split[w][(*i) + 1] != ' ')
			{
				(*i)++;
				n->command[(*k)++] = n->input_split[w][(*i)];
			}
		}
	}
	if (n->input_split[w][*i])
	{
		if(n->dollars % 2 == 0 && is_quoted(n, w) != 1 && env_n == -1
			&& (!n->input_split[w][n->idollar] || n->input_split[w][n->idollar] == ' ')) //"$ a", $, 
			n->command[(*k)++] = '$';
		(*i)++;
	}
	return (env_n);
}




void	save_rest_no_env(t_input *in, size_t w, size_t *i, size_t *k)
{
	size_t	j;
	int		is_digit;
	char	*str;

	j = (*i);
	str = in->input_split[w];
	is_digit = ft_isdigit(str[in->idollar]);
	if (str[(*i)] && str[(*i) + 1] == ' ')
	{
		while (str[j] != '$')
		{
			if (is_digit || ft_strrchr(N_ODDCHAR, str[j])
					|| ft_strrchr(D_Y_ODDCHAR, str[j]))
				break ;
			j--;
		}
		if (str[j] != '$')
			in->command[(*k)++] = in->input_split[w][(*i)];
	}
}


void	save_rare_cases(t_input *in, size_t w, size_t *i, size_t *k)
{
	char	*str;
	size_t	index;
	
	index = in->idollar;
	str = in->input_split[w];
	if (in->dollars % 2)// && in->input_split[w][*i])///anadido que sea distinto de cero
	{
		in->command[(*k)++] = str[index];
		(*i) = index;/////////////
		return ;
	}
	if (!str[index] && in->input_split[w][*i]) ///anadido que sea distinto de cero
		in->command[(*k)++] = '$';
	else if ((ft_isdigit(str[index]) || ft_strrchr(N_ODDCHAR, str[index])
		|| ft_strrchr(D_Y_ODDCHAR, str[index])) && str[index])
	{
		if (str[index] && !str[index + 1] && !ft_strrchr(D_Y_ODDCHAR, str[index]))
			in->spaced = 0;
		if (ft_strrchr(D_Y_ODDCHAR, str[index]))
		{
			in->command[(*k)++] = '$';
			in->command[(*k)++] = str[index];
		}	
		(*i) = index;
	}
}

/*echo $$USER will print USER. $$USERp will be USERp. That is the first while*/
/*$ will be printed only if $ are ODD (1,3,5...). Here first $ is not...*/
/*...counted so to be ODD it has to be a EVEN number. That is the reason...*/
/*...of !(in->dollars % 2). BUT also can only be printed if after the $...*/
/*...(so idollar index) is not a possible valid ENV (starting with alpha...*/
/*...or a '_'.*/
/*env_n = -2 is when there is an $? so print the exit code. It behaves as...*/
/*...BASH where echo $???msg will be (exit number)??msg*/
/*in->spaced is necesary to beginning with tokens that not has to be spaced...*/
/*...even if they are, because the are preceeded from invalid tokens not...*/
/*...not printed. The read of that variable is at begging of parsing*/
void	do_even_dollars(t_input *in, size_t w, size_t *i, size_t *k)
{
	size_t	j;

	j = 0;
	while (in->input_split[w][*i] && in->input_split[w][(*i) + 1] != ' '
		&& in->input_split[w][*i] != '$' && in->env_n != -2)
	{
		if (is_quoted(in, w) == 2 && in->input_split[w][*i] == '\'')
		{
			if (*i > 0)
				(*i)--;
			return ;
		}
		if (in->dollars > 0 && (in->dollars % 2) && (j < in->env_len))
			in->command[(*k)++] = in->input_split[w][(*i)];
		else if (j >= in->env_len 
			&& !ft_isdigit(in->input_split[w][in->idollar])
			&& !ft_strrchr(D_Y_ODDCHAR, in->input_split[w][in->idollar])
			&& !ft_strrchr(N_ODDCHAR, in->input_split[w][in->idollar]))
		{
			in->spaced = 1;
			in->command[(*k)++] = in->input_split[w][(*i)];
		}	
		(*i)++;
		j++;
	}
}

/*will print the last exit code when $? only when number of ? is ODD*/
/*I dont count the first $ encounter so that is the reason of...*/
/*...if (!(in->dollars % 2))*/
void	env_question(t_input *in, size_t w, size_t *i, size_t *k)
{
	char	*number;
	size_t	l;
	
	l = 0;
	(*i)++;
	if (!(in->dollars % 2))
	{
		number = ft_itoa(in->last_exit_code);
		if (!number)
			clean_all(in, 1);
		while (number[l])
			in->command[(*k)++] = number[l++];
		free(number);
	}
	else 
		in->command[(*k)++] = '?';
	while (in->input_split[w][*i] && in->input_split[w][(*i) + 1] != ' '
		&& in->input_split[w][*i] != '$')
		in->command[(*k)++] = in->input_split[w][(*i)++];
}

void	save_invalid_envs(t_input *in, size_t w, size_t *i, size_t *k)
{
	if (in->env_n > -1)
		in->env_len = validlen_env(in->envp[in->env_n], '=');
	else if (in->env_n == -1)
		in->env_len = invalidlen_env(in->input_split[w] + (*i));
	do_even_dollars(in, w, i, k);
	if (in->env_n < 0
		&& (ft_isalpha(in->input_split[w][in->idollar])
		|| in->input_split[w][in->idollar] == '_'))
		save_rest_no_env(in, w, i, k);
	else if (in->env_n == -1)
		save_rare_cases(in, w, i, k);
	else if (in->env_n == -2)
		env_question(in, w, i, k);
}

void	save_if_spaced_and_valid_env(t_input *in, size_t w)
{
	size_t	i;
	int		env_n;
	size_t	idollar;

	i = 0;
	in->dollars = 0;
	while (in->input_split[w][i] != '$')
		i++;
	idollar = i;
	while (in->input_split[w][i] == '$')
	{
		(in->dollars)++;
		i++;
	}
	env_n = valid_env((in->input_split[w] + i), in, w);
	if (env_n == -1
		&& (ft_isalpha(in->input_split[w][i]) || in->input_split[w][i] == '_')
		&& !idollar
		&& is_quoted(in, w) != 2 && in->dollars == 1)
		space_after_first_invalid_env(in, w);
}

void	expand_dollar(t_input *in, size_t *i, size_t *j, size_t *k)
{
	save_if_spaced_and_valid_env(in, *i);
	in->dollars = 0;
	while (in->input_split[*i][*j])
	{
		if (in->input_split[*i][*j] != '$')
			in->command[(*k)++] = in->input_split[*i][(*j)];
		else
		{
			while (in->input_split[*i][(*j) + 1] == '$')
			{
				(*j)++;
				(in->dollars)++;
			}
			in->idollar = (*j) + 1;
			in->env_n = save_valid_env_variable(in, *i, j, k);
			save_invalid_envs(in, *i, j, k);
		}
		if (in->input_split[*i][*j] && in->input_split[*i][*j] != '$')
			(*j)++;
	}
}
