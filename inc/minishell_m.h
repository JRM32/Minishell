/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell_m.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mpico-bu <mpico-bu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 11:51:03 by mpico-bu          #+#    #+#             */
/*   Updated: 2025/05/02 00:58:41 by mpico-bu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_M_H
# define MINISHELL_M_H

# include <stdio.h>
# include <stdlib.h>
# include <stdbool.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "../libft/libft.h"

void	ft_manage_history(char *input, bool clean);
void	clean_all(void);

//BUILT INS
void	ft_pwd(char **input);
void	ft_env(char **input, char **envp);
void	ft_echo(char **input);
void	ft_export(char *input, char ***envp);
void	ft_unset(char *input, char ***envp);
bool	ft_cd(char **args, char **envp);

void	execute_command(char *cmd, int input_fd, int output_fd, char **envp);

#endif
