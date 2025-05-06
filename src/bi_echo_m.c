/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bi_echo_m.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrollon- <jrollon-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 18:50:16 by mpico-bu          #+#    #+#             */
/*   Updated: 2025/05/06 09:35:23 by jrollon-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell_m.h"
#include "../inc/minishell_j.h"

/* void	compose_arg(char **sp, int *st, t_input *in, size_t w)
{
	size_t	i;
	size_t	j;
	size_t	k;
	int		qw;
	int		st_done;

	k = 0;
	i = w;
	qw = 0;
	st_done = 0;
	ft_bzero(in->args, 100);
	while (i < in->input_words)
	{
		j = 0;
		st = 0;
		if (i > w && sp[i][0] == '\0' && (st[i] == SQUO_SP || st[i] == DQUO_SP))
			break ;
		while ((sp[i][j] != ' ' || is_quoted(in, i)) && sp[i][j] != '\0' && !qw)
		{
			if ((i > w && (st[i] == EPTY_SP || st[i] == SQUO_SP 
				|| st[i] == DQUO_SP) && !st_done))
				qw = 1;
			else
				in->args[k++] = sp[i][j++];
			st_done = 1;
		}
		if (in->input_split[i++][j] == ' ' || qw)
			break ;

	}
	in->word_after_arg = i;
} */




/*start is because the first argument from the command or the -n even it is...*/
/*...spaced it dont have to space it when printed.*/
void	ft_echo(t_input *in)
{
	size_t	i;
	//size_t	j;
	size_t	n_repeated;
	int		argument;
	size_t	start;
	
	i = 1;
	n_repeated = 1;
	while (in->args[i])
	{
		if (in->args[i++] != 'n')
			n_repeated = 0;
	}
	argument = ft_strncmp(in->args, "-n", 2);
	if (argument == 0 && n_repeated)
	{
		i = in->word_after_arg;
		/* j = i + 1;
		n_repeated = 1;
		while (j < in->input_words)
		{
			while (in->input_split[j])
			{
				
				if (in->input_split[j] != 'n')
					
			}
			j++;
		} */
	}
	else
		i = in->word_after_command;
	start = i;
	while (i < in->input_words && in->input_split && in->input_split[i])
	{
		if ((in->status[i] == EPTY_SP || in->status[i] == SQUO_SP
			|| in->status[i] == DQUO_SP) && in->input_split[i][0]
			&& i != start)
			printf(" %s", in->input_split[i]);
		else if ((in->status[i] == EPTY_SP || in->status[i] == SQUO_SP
			|| in->status[i] == DQUO_SP) && in->input_split[i][0] == '\0'
			&& i != start)
			printf(" ");
		else if (in->input_split[i][0])
			printf("%s", in->input_split[i]);
		i++;
	}
	if (argument != 0)
		printf("\n");
}
