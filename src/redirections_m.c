/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections_m.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mpico-bu <mpico-bu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 01:26:18 by mpico-bu          #+#    #+#             */
/*   Updated: 2025/05/29 04:04:17 by mpico-bu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell_m.h"
#include "../inc/minishell_j.h"

/*
void remove_redirections_from_input(t_input *input)
{
	char	*clean_input;
	size_t	i = 0, j = 0;
	bool	sq = false, dq = false;

	clean_input = malloc(ft_strlen(input->input) + 1);
	while (input->input[i])
	{
		if (input->input[i] == '\'' && !dq)
			sq = !sq;
		else if (input->input[i] == '"' && !sq)
			dq = !dq;
		else if (!sq && !dq && (input->input[i] == '<' || input->input[i] == '>'))
		{
			size_t skip = 1;
			if (input->input[i + 1] == '<' || input->input[i + 1] == '>')
				skip++;
			i += skip;
			while (input->input[i] == ' ')
				i++;
			while (input->input[i] && input->input[i] != ' ' && input->input[i] != '<' && input->input[i] != '>')
			{
				if (input->input[i] == '"' || input->input[i] == '\'')
					break;
				i++;
			}
			continue;
		}
		clean_input[j++] = input->input[i++];
	}
	clean_input[j] = '\0';
	free(input->input);
	input->input = clean_input;
}


static void	handle_append_redirection(t_input *input, char *redir)
{
	char	*filename;
	char	*end;

	*redir = '\0';
	redir += 2;
	while (*redir == ' ')
		redir++;
	filename = ft_strdup(redir);
	end = filename + ft_strlen(filename) - 1;
	while (end > filename && (*end == ' ' || *end == '\n'))
		*end-- = '\0';
	input->outputfd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	free(filename);
}

static void	handle_output_redirection(t_input *input, char *redir)
{
	char	*filename;
	char	*end;

	*redir = '\0';
	redir++;
	while (*redir == ' ')
		redir++;
	filename = ft_strdup(redir);
	end = filename + ft_strlen(filename) - 1;
	while (end > filename && (*end == ' ' || *end == '\n'))
		*end-- = '\0';
	input->outputfd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	free(filename);
}

static void	handle_input_redirection(t_input *input, char *redir)
{
	char	*filename;
	char	*end;

	*redir = '\0';
	redir++;
	while (*redir == ' ')
		redir++;
	filename = ft_strdup(redir);
	end = filename + ft_strlen(filename) - 1;
	if ((filename[0] == '"' && end[0] == '"') || (filename[0] == '\'' && end[0] == '\''))
	{
		filename++;
		end--;
		*end = '\0';
	}
	while (end > filename && (*end == ' ' || *end == '\n'))
		*end-- = '\0';
	input->inputfd = open(filename, O_RDONLY);
	free(filename);
}


*/


int set_input_redirection(t_input *input, const char *filename) {
    // Abrimos el archivo en modo solo lectura
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("Error abriendo archivo de entrada");
        return 0;
    }

    input->inputfd = fd;

    // Eliminar la parte "<archivo" de input->input
    char *redir_pos = strstr(input->input, "<");
    if (redir_pos) {
        *redir_pos = '\0';  // Corta justo antes de la redirección
        size_t len = strlen(input->input);
        while (len > 0 && input->input[len - 1] == ' ') {
            input->input[len - 1] = '\0';
            len--;
        }
    }
	printf("Input: %s\n", input->input);
	
	ft_matrix_free(&input->input_split);
	
	input->input_split = ft_split_quotes(input->input, ' ', input);
	
	free(input->parsed);
	char *space = strchr(input->input, ' ');
	if (space != NULL) {
		// Saltar espacios adicionales después del comando
		while (*space == ' ')
			space++;
		input->parsed = ft_strdup(space);
	} else {
		// No hay argumentos, dejar parsed vacío
		input->parsed = ft_strdup("");
	}

	printf("Parsed: %s\n", input->parsed);
	
	ft_matrix_free(&input->split_exp);
    input->split_exp = ft_matrix_dup(input->input_split);
	for(int j = 0; input->split_exp[j]; j++)
		printf("Split exp %d: %s\n", j, input->split_exp[j]);
	printf("Redir in: %i\n", input->inputfd);
    return 1;
}

bool handle_redirection(t_input *input) {
    for (int i = 0; input->split_exp[i]; i++) {
        char *token = input->split_exp[i];

        // Caso: "<" separado del nombre del archivo
        if (strcmp(token, "<") == 0 && input->split_exp[i + 1]) {
            return set_input_redirection(input, input->split_exp[i + 1]);
        }

        // Caso: "<archivo" todo junto
        if (token[0] == '<' && strlen(token) > 1) {
            return set_input_redirection(input, token + 1);
        }
    }
    return true;
}