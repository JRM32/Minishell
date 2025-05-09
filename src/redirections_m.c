/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections_m.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mpico-bu <mpico-bu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 01:26:18 by mpico-bu          #+#    #+#             */
/*   Updated: 2025/05/10 01:02:47 by mpico-bu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell_m.h"
#include "../inc/minishell_j.h"

void handle_redirection(t_input *input)
{
    char *redir;
    int pipefd[2];
    char *filename;
    char *buffer;

    // 1. Append Redirection (>>)
    if ((redir = ft_strnstr(input->input, ">>", ft_strlen(input->input))))
    {
        *redir = '\0';
        redir += 2;
        while (*redir == ' ') redir++;
        filename = ft_strdup(redir);
        char *end = filename + strlen(filename) - 1;
        while (end > filename && (*end == ' ' || *end == '\n'))
            *end-- = '\0';

        input->outputfd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (input->outputfd < 0)
            perror("open append");
        free(filename);
    }
    // 2. Output Redirection (>)
    else if ((redir = ft_strchr(input->input, '>')))
    {
        *redir = '\0';
        redir++;
        while (*redir == ' ') redir++;
        filename = strdup(redir);
        char *end = filename + strlen(filename) - 1;
        while (end > filename && (*end == ' ' || *end == '\n'))
            *end-- = '\0';

        input->outputfd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (input->outputfd < 0)
            perror("open output");
        free(filename);
    }
    // 3. Heredoc Redirection (<<)
 else if ((redir = ft_strnstr(input->input, "<<", ft_strlen(input->input))))
{
    *redir = '\0';
    redir += 2;
    while (*redir == ' ') redir++;

    // Capturar el delimitador del heredoc
    char *delim = ft_strdup(redir);
    char *end = delim + ft_strlen(delim) - 1;
    while (end > delim && (*end == ' ' || *end == '\n'))
        *end-- = '\0';

    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        free(delim);
        return;
    }
    printf("heredoc> ");
    buffer = malloc(8192);
    buffer[0] = '\0';
    char line[1024];
    while (fgets(line, sizeof(line), stdin))
    {
        // Remover salto de línea
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, delim) == 0)
            break;
        ft_strcat(buffer, line);
        ft_strcat(buffer, "\n");
        printf("heredoc> ");
    }

    write(pipefd[1], buffer, ft_strlen(buffer));
    close(pipefd[1]);

    input->inputfd = pipefd[0];

    free(buffer);
    free(delim);
}


    // 4. Input Redirection (<)
    else if ((redir = ft_strchr(input->input, '<')))
    {
        *redir = '\0';
        redir++;
        while (*redir == ' ') redir++;
        filename = ft_strdup(redir);
        char *end = filename + strlen(filename) - 1;
        while (end > filename && (*end == ' ' || *end == '\n'))
            *end-- = '\0';
        input->inputfd = open(filename, O_RDONLY);
        if (input->inputfd < 0)
            perror("open input");
        free(filename);
    }
}