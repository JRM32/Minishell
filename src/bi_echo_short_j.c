/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bi_echo_short_j.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrollon- <jrollon-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 17:24:00 by jrollon-          #+#    #+#             */
/*   Updated: 2025/05/18 12:52:33 by jrollon-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell_m.h"
#include "../inc/minishell_j.h"

size_t	check_more_n(t_input *in)
{
	size_t	i;
	size_t	j;
	size_t	n_repeated;

	n_repeated = 1;
	j = in->word_after_arg;
	while (j < in->input_words)
	{
		i = 1;
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


size_t	check_argument(t_input *in)
{
	size_t	i;
	size_t	n_repeated;
	
	i = 0;
	n_repeated = 1;
	if (in->parsed && in->parsed[0] != '-')
		return (0);
	else
	{
		/* if (in->parsed[1] != 'n')
			return (0); */
		i = 1;
		while (in->parsed[i] && in->parsed[i] == 'n')
			i++;
			
		
	}

	
	
	

	if (ft_strncmp(in->parsed, "-n", 2) != 0)
		in->echo_error_n_arg = 1;
	if (in->echo_error_n_arg == 0 && n_repeated)
		i = check_more_n(in);
	else
		i = in->word_after_command;
	return (i);
}












void	echo_short(t_input *in)
{
	size_t	start;
	
	start = check_argument(in);

	ft_printf("start: %zu\n", start);
	
	/*esta logica funciona para echos lanzados en terminal. no para $kk (echo oculto)*/
	ft_printf("%s", in->parsed);
	if (in->echo_error_n_arg == 1)
		ft_printf("\n");
	
	/* cuando se hace $kk donde kk es echo, el -n viene de la variable. NO
	de el argumento que pasemos como $kk -n
	export kk="echo -nnn -na      msg1    " -> $kk msg2 = -na msg1 msg2(SIN \n)
	export kk="echo msg1" -> $kk -n msg2 = msg1 -n msg2	
	
	no me puedo fiar del in->arg ya que en donde no lo hace no pone -n, pero donde 
	lo hace no lo pone, o sea tengo que buscar el -n en el in->parsed
	TENGO QUE BUSCAR UNA MANERA de cambiar a esta logica cuando el echo venga de una
	variable de entorno... es decir este lanzada por un $VAR
	
	1. cuando se lanza un comando como $ se puede hacer como ''$

	
	*/

	
}
