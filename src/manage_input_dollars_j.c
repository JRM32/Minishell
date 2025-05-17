/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   manage_input_dollars_j.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrollon- <jrollon-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 12:59:27 by jrollon-          #+#    #+#             */
/*   Updated: 2025/05/17 14:52:00 by jrollon-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell_m.h"
#include "../inc/minishell_j.h"

/*will print the env if $USER or $$$USER. with $$USER will print USER that...*/
/*...is inside the else condition. that will stop up to the \0 or ' ' (if...*/
/*...inside of double quotes)*/
void	save_env_if_even_dollars(t_input *in, size_t w, size_t *i, size_t *k)
{
	size_t	j;
	
	if (!(in->dollars % 2))
	{
		j = 0;
		while (in->envp[in->env_n][j] != '=')
			j++;
		j++;
		while (in->envp[in->env_n][j] == ' ')
			j++;
		while (in->envp[in->env_n][j])
		{
			in->command[(*k)++] = in->envp[in->env_n][j++];
			if (in->envp[in->env_n][j] == ' ')
				break ;
		}
	}
	else
	{
		while (in->input_split[w][*i] && in->input_split[w][(*i) + 1] != ' ')
		{
			(*i)++;
			in->command[(*k)++] = in->input_split[w][(*i)];
		}
	}
}

/*the second if is for cases like "$ a", $, where we are after the $ but...*/
/*...inside of double quotes or just nothing more, or course with EVEN $...*/
/*...again n->dollars % 2 == 0 is because we dont count the first $ found*/
void	save_valid_env_variable(t_input *n, size_t w, size_t *i, size_t *k)
{
	n->env_n = valid_env((n->input_split[w] + (*i) + 1), n, w);
	if (n->env_n > -1)
		save_env_if_even_dollars(n, w, i, k);
	if (n->input_split[w][*i])
	{
		if(n->dollars % 2 == 0 && is_quoted(n, w) != 1 && n->env_n == -1
			&& (!n->input_split[w][n->idollar] 
				|| n->input_split[w][n->idollar] == ' ')) 
			n->command[(*k)++] = '$';
		(*i)++;
	}
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


/*save cases as $@p msg -> p msg or $%p msg -> $%p msg*/
/*when echo $$2p will save 2p. That is the (in->dollars % 2) case...*/
/*...it is ODD as we dont count the first $. So $$$$ are 3*/
void	save_rare_cases(t_input *in, size_t w, size_t *i, size_t *k)
{
	char	*str;
	size_t	id;
	
	id = in->idollar;
	str = in->input_split[w];
	if (in->dollars % 2)
	{
		in->command[(*k)++] = str[id];
		(*i) = id;
		return ;
	}
	if (!str[id] && in->input_split[w][*i])
		in->command[(*k)++] = '$';
	else if ((ft_isdigit(str[id]) || ft_strrchr(N_ODDCHAR, str[id])
			|| ft_strrchr(D_Y_ODDCHAR, str[id])) && str[id])
	{
		if (str[id] && !str[id + 1] && !ft_strrchr(D_Y_ODDCHAR, str[id]))
			in->spaced = 0;
		if (ft_strrchr(D_Y_ODDCHAR, str[id]))
		{
			in->command[(*k)++] = '$';
			in->command[(*k)++] = str[id];
		}	
		(*i) = id;
	}
}

/* echo $$USER will print USER. $$USERp will be USERp.*/
/* $ will be printed only if $ are ODD (1,3,5...). Here the first $ is not...*/
/* ...counted, so to be ODD it has to be an EVEN number of dollars. That is...*/
/* ...the reason of !(in->dollars % 2). BUT it can only be printed if the char*/
/* ...after the $ (at idollar index) is not a possible valid ENV (not alpha...*/
/* ...or '_', nor special treated chars).*/
/* env_n = -2 is when there is a $? so it will print the exit code. It behaves*/
/* ...as in BASH where echo $???msg will be (exit number)??msg*/
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
			save_valid_env_variable(in, *i, j, k);
			save_invalid_envs(in, *i, j, k);
		}
		if (in->input_split[*i][*j] && in->input_split[*i][*j] != '$')
			(*j)++;
	}
}
