/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exe_commands.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leu-lee <leu-lee@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/19 16:40:15 by leu-lee           #+#    #+#             */
/*   Updated: 2022/04/30 18:33:10 by leu-lee          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	exe_path(char **input, char **envp, char **path)
{
	int		i;
	char	*cmd;

	if (execve(input[0], input, envp) == -1)
	{
		i = 0;
		while (path[i])
		{
			cmd = join_key_value(path[i++], input[0], '/');
			execve(cmd, input, envp);
			free(cmd);
		}
	}
	ft_putstr_fd(input[0], 2);
	ft_putstr_fd(": command not found\n", 2);
	g_exit = 127;
	exit(g_exit);
}

void	exe_execve(t_cmd_grp *cmd_grp, t_data *data)
{
	char	**envp;
	char	**path;
	char	*mini_env;

	mini_env = mini_getenv("PATH", data->envp);
	envp = get_env_array(data);
	if (mini_env == NULL)
	{
		execve(cmd_grp->args[0], cmd_grp->args, envp);
		ft_putstr_fd(cmd_grp->args[0], 2);
		ft_putstr_fd(": No such file or directory\n", 2);
		exit (1);
	}
	path = ft_split(mini_env, ':');
	exe_path(cmd_grp->args, envp, path);
	free_str_array(path);
	free_str_array(envp);
}

void	exe_commands(t_cmd_grp *cmd_grp, t_data *data, int pipe_num)
{
	int	status;

	if (pipe_num == 0)
	{
		g_exit = 0;
		if (exe_builtins(cmd_grp, data) == 1)
		{
			if (ft_fork() == 0)
			{
				exe_execve(cmd_grp, data);
				exit(1);
			}
			waitpid(-1, &status, 0);
			if (WIFEXITED(status))
				g_exit = WEXITSTATUS(status);
		}
	}
	else
	{
		if (exe_builtins(cmd_grp, data) == 1)
			exe_execve(cmd_grp, data);
	}
}
