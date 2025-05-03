/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split_quotes_separator.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrollon- <jrollon-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 16:21:17 by jrollon-          #+#    #+#             */
/*   Updated: 2025/05/03 16:53:19 by jrollon-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "../inc/minishell_j.h"

void	init_separator(t_split *sq, size_t *i, size_t *j)
{
	int	escaped;
	
	*i = 0;
	*j = 0;
	escaped = is_escaped(sq, i);//
	if (sq->s[sq->start] == '"' && !(sq->quotes % 2) && !escaped)//
	{
		sq->c = '"';
		(sq->quotes)++;
	}
	else if (sq->s[sq->start] == '\'' && !(sq->quotes % 2) && !escaped)//
	{
		sq->c = '\'';
		(sq->quotes)++;
	}
	else if ((sq->s[sq->start] == '\'' || sq->s[sq->start] == '"') && !escaped)//
	{
		sq->c = ' ';
		(sq->quotes)++;
		(sq->start)++;
	}
}

void	run_spaces_or_one_quote(t_split *sq, t_input *input)
{
	input->spaced = 0;
	input->escaped = 0;//
	if (sq->start != 0 && sq->s[(sq->start) - 1] == ' ')
		input->spaced = 1;
	if (sq->c != '"' && sq->c != '\'')
	{
		while ((sq->s[sq->start] == sq->c) && (sq->s[sq->start]))
			(sq->start)++;
		if (sq->start != 0 && sq->s[(sq->start) - 1] == ' ')
			input->spaced = 1;
	}
	else if (sq->c == '"' || sq->c == '\'')
		(sq->start)++;
}

void	open_close_quotes(t_split *squotes)
{
	if (squotes->s[squotes->start] == '"' && !(squotes->quotes % 2))
	{
		squotes->c = '"';
		(squotes->quotes)++;
		(squotes->start)++;
	}
	else if (squotes->s[squotes->start] == '\'' && !(squotes->quotes % 2))
	{
		squotes->c = '\'';
		(squotes->quotes)++;
		(squotes->start)++;
	}
	else if (squotes->c == '"' && squotes->s[squotes->start] == '"')
		(squotes->quotes)++;
	else if (squotes->c == '\'' && squotes->s[squotes->start] == '\'')
		(squotes->quotes)++;
}

void	compose_split_aux(t_split *squotes, size_t *i, size_t *j)
{
	while (*j < *i)
		squotes->split_aux[(*j)++] = squotes->s[(squotes->start)++];
	if (squotes->c == ' ')
	{
		while (squotes->s[squotes->start]
			&& squotes->s[squotes->start] == squotes->c)
			(squotes->start)++;
	}
	squotes->c = ' ';
}

char	*sub_split_quotes(t_split *sq, t_input *input)
{
	size_t	i;
	size_t	j;

	init_separator(sq, &i, &j);
	run_spaces_or_one_quote(sq, input);
	open_close_quotes(sq);
	while (sq->s[i + sq->start] && sq->s[i + sq->start] != sq->c)
	{
		if (sq->c == ' ' && (sq->s[i + sq->start] == '"'
				|| sq->s[i + sq->start] == '\''))
			break ;
		i++;
	}
	sq->split_aux = (char *)ft_calloc(i + 1, sizeof(char));
	if (!sq->split_aux)
		return (NULL);
	if ((sq->c == '"' && sq->s[sq->start] == '"')
		|| (sq->c == '\'' && sq->s[sq->start] == '\''))
		(sq->start)++;
	compose_split_aux(sq, &i, &j);
	return (sq->split_aux);
}
