/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   manage_inputs_m.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mpico-bu <mpico-bu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 19:28:00 by mpico-bu          #+#    #+#             */
/*   Updated: 2025/05/22 16:56:23 by mpico-bu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell_m.h"
#include "../inc/minishell_j.h"

//printf("command :%s\n", input->command);//
//printf("arg :%s\n-------------\n", input->args);//

void	ft_manage_input(t_input *input, int in_fd, int out_fd)
{
	input->inputfd = in_fd;
	input->outputfd = out_fd;
	if (handle_redirection(input) == 1)
		input->inputfd = in_fd;
	else if (ft_strcmp(input->command, "pwd") == 0)
		ft_pwd(input->args);
	else if (ft_strcmp(input->command, "cd") == 0)
		ft_cd(input);
	else if (ft_strcmp(input->command, "echo") == 0)
		echo_short(input);
	else if (ft_strcmp(input->command, "export") == 0)//mirar cambiar a && input->args[0] (es decir que no sea \0). Pero!! export sin mas lo que hace es poner "declare -x " delante de todas las variables de entorno
		//ft_export(input, &input->envp);
		ft_export(input->parsed, &input->envp);//este funciona
	else if (ft_strcmp(input->command, "env") == 0)
		ft_env(input, input->envp);
	else if (ft_strcmp(input->command, "unset") == 0)//mirar cambiar a && input->args[0] (es decir que no sea \0). Pero!! unset sin mas lo que hace es poner una nueva linea de prompt
		ft_unset(input);
	else if (ft_strcmp(input->command, "exit") == 0)
		ft_exit(input);
	else
		execute_command(input);
	input->last_dollar_ = ft_last_str(input->split_exp);
	ft_input_free(input);
}
