/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_j.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrollon- <jrollon-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 17:33:24 by jrollon-          #+#    #+#             */
/*   Updated: 2025/05/14 09:12:43 by jrollon-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell_m.h"
#include "../inc/minishell_j.h"

/* void	create_node_parse(t_input *in)
{
	t_pars	*aux;
	t_pars	*new;
	
	new = (t_pars *)ft_calloc(1, sizeof(t_pars));
	if (!new)
	{
		clean_all(in);
		exit (1);//mirar si salgo con exit o no.
	}
	if (!in->parsed)
	{
		in->parsed = new;
		return ;
	}
	aux = in->parsed;
	while (aux->next)
		aux = aux->next;
	aux->next = new;
}


void	create_parse(t_input *in, size_t *i, size_t *j, size_t *k)
{
	size_t	start;
	size_t	n_chars;

	n_chars = 0;
	start = (*i);
	create_node_parse(in);
	while (*i < in->input_words)
	{
		*j = 0;
		in->status_checked = 0;
		if (*i > 0 && in->input_split[*i][0] == '\0'
			&& (in->status[*i] == SQUO_SP || in->status[*i] == DQUO_SP))
			break ;
		while ((in->input_split[*i][*j] != ' ' || is_quoted(in, *i))
			&& (in->input_split[*i][*j] != '\0') && !in->spaced)
		{
			if (*i > 0 && (in->status[*i] == EPTY_SP 
					|| in->status[*i] == SQUO_SP
					|| in->status[*i] == DQUO_SP)
				&& !in->status_checked)
				in->spaced = 1;
			else
			{
				(*j)++;
				n_chars++;
			}	
			in->status_checked = 1;
		}
		if (in->input_split[*i][*j] == ' ' || in->spaced)
			break ;
		(*i)++;
	}
	in->parsed->str = (char *)ft_calloc(n_chars + 1, sizeof(char));
	if (!in->parsed->str)
	{
		clean_all(in);
		exit(1);
	}
	*j = 0;
	while ((*k) < n_chars)
	{
		*j = 0;
		while (in->input_split[start][*j])
			in->parsed->str[(*k)++] = in->input_split[start][(*j)++];
		start++;
	}
}


void	parsing(t_input *in)
{
	size_t	i;
	size_t	j;
	size_t	k;

	k = 0;
	i = 0;
	in->spaced = 0;
	in->status_checked = 0;
	in->parsed = NULL;
	while (i < in->input_words)
	{
		create_parse(in, &i, &j, &k);
		k = 0;
		in->word = i;
		in->spaced = 0;
	}
	
	
	
	
	t_pars *aux = in->parsed;
	size_t	contador = 0;
	while (aux)
	{
		printf("%zu: %s\n", contador, aux->str);
		contador++;
		aux = aux->next;
	}
} */





size_t	p_check_rest_of_n(t_input *in)
{
	size_t	i;
	size_t	j;
	size_t	n_repeated;

	n_repeated = 1;
	j = in->word_after_arg;
	while (j < in->input_words)
	{
		i = 1;
		compose_arg(in, j);
		while (in->args[i] && n_repeated)
		{
			if (in->args[0] != '-' || in->args[i++] != 'n')
			{
				in->word_after_arg--;
				n_repeated = 0;
			}
		}
		if (ft_strncmp(in->args, "-n", 2) != 0)
			n_repeated = 0;
		if (n_repeated == 0)
			break ;
		j++;
	}
	return (j);
}

/*We check if there is a valid -n or -nnnnnnnn (all n) in the first...*/
/*...word after the command. If it is, then we check rest of possible...*/
/*... -nnnn -n that would produce the same result. ONLY the first one...*/
/*... will be the one that rules if there is an -n non \n print in ft_echo*/
size_t	p_give_me_the_fist_word(t_input *in, int *error_argument)
{
	size_t	i;
	size_t	n_repeated;

	i = 1;
	n_repeated = 1;
	while (in->args[i])
	{
		if (in->args[i++] != 'n' || in->args[0] != '-')
		{
			n_repeated = 0;
			*error_argument = 1;
		}
	}
	if (ft_strncmp(in->args, "-n", 2) != 0)
		*error_argument = 1;
	if (*error_argument == 0 && n_repeated)
		i = p_check_rest_of_n(in);
	else
		i = in->word_after_command;
	return (i);
}

/*print_as_env is 1 when it is "$USER" or $USER because '$USER' will print...*/
/*...just what is inside the ''*/
void	p_print_arguments(t_input *in, size_t	w, int spaced)
{
	int		print_as_env;

	in->spaced = 1;
	if (spaced == -1)
	{
		printf(" ");
		return ;
	}
	print_as_env = (is_quoted(in, w) == 2 || !is_quoted(in, w));
	if (spaced)
	{
		if (ft_strrchr(in->input_split[w], '$') && print_as_env)
			p_manage_dollar(in, w, 1);
		else
			printf(" %s", in->input_split[w]);
	}
	else
	{
		if (ft_strrchr(in->input_split[w], '$') && print_as_env)
			p_manage_dollar(in, w, 0);
		else
			printf("%s", in->input_split[w]);
	}
}

/*start is because the first argument from the command or the -n even it is...*/
/*...spaced it dont have to space it when printed.*/
void	init_parsing(t_input *in)
{
	size_t	i;
	size_t	start;

	in->echo_error_n_arg = 0;
	start = p_give_me_the_fist_word(in, &(in->echo_error_n_arg));
	i = start;
	while (i < in->input_words && in->input_split && in->input_split[i])
	{
		if ((in->status[i] == EPTY_SP || in->status[i] == SQUO_SP
				|| in->status[i] == DQUO_SP) && in->input_split[i][0]
				&& i != start && in->spaced)
			p_print_arguments(in, i, 1);
		else if ((in->status[i] == EPTY_SP || in->status[i] == SQUO_SP
				|| in->status[i] == DQUO_SP) && in->input_split[i][0] == '\0'
				&& i != start)
			p_print_arguments(in, i, -1);
		else if (in->input_split[i][0])
			p_print_arguments(in, i, 0);
		i++;
	}
}




void	parsing(t_input *in)
{
	init_parsing(in);
	
}
