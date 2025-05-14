/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_j.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrollon- <jrollon-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 17:33:24 by jrollon-          #+#    #+#             */
/*   Updated: 2025/05/14 14:05:10 by jrollon-         ###   ########.fr       */
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

char *choose_name(void)
{
	int		fd;
	int		i;
	char	*number;
	char 	*filename;

	i = 0;
	if (access(".", W_OK | R_OK) == -1)
		return (NULL);
	number = ft_itoa(i);
	if (!number)
		return (NULL);
	filename = ft_strjoin("temp", number);
	if (!filename)
		return (NULL);
	fd = open(filename, O_RDONLY, 0644);
	while (fd != -1)
	{
		i++;
		free(filename);
		free(number);
		number = ft_itoa(i);
		if (!number)
			return (close(fd), NULL);
		filename = ft_strjoin("temp", number);
		if (!filename)
			return (close(fd), free(number), NULL);
		fd = open(filename, O_RDONLY, 0644);	
	}
	free(number);
	return (filename);
}




void	parsing(t_input *in)
{
	int		fd;
	int		stdout_save;
	
	stdout_save = dup(STDOUT_FILENO);
	in->filename = choose_name();
	if (stdout_save == -1 || !in->filename)
	{
        if (!in->filename && stdout_save != 1)
			close(stdout_save);
		clean_all(in);
        exit(1);
    }
	fd = open(in->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		close(stdout_save);
		clean_all(in);
		exit (1);
	}
	if(dup2(fd, STDOUT_FILENO) == -1)
	{
        close(fd);
        close(stdout_save);
        clean_all(in);
        exit(1);
	}

	ft_echo(in, 0);


	if (dup2(stdout_save, STDOUT_FILENO) == -1)
	{
		close(fd);
		close(stdout_save);
		clean_all(in);
		exit(1);
	}  
	close(fd);
	close(stdout_save);
}
