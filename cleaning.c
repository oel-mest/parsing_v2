/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleaning.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oel-mest <oel-mest@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 22:48:30 by oel-mest          #+#    #+#             */
/*   Updated: 2025/03/19 00:15:08 by oel-mest         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_env_list(EnvNode *env_list)
{
	EnvNode *next;
	
	while (env_list)
	{
		next = env_list->next;
		free(env_list->var);
		free(env_list->value);
		free(env_list);
		env_list = next;
	}
}

void	free_tokens(t_token *tokens)
{
	t_token *next;
	
	while (tokens)
	{
		next = tokens->next;
		free(tokens->value);
		free(tokens);
		tokens = next;
	}
}

void	free_output_list(t_output *output_list)
{
	t_output *next;
	
	while (output_list)
	{
		next = output_list->next;
		if (output_list->file != NULL)
			free(output_list->file);
		if (output_list != NULL)
			free(output_list);
		output_list = next;
	}
}

void	free_redi(t_redi *redi)
{
	if (!redi)
		return;
	free_output_list(redi->input);
	free_output_list(redi->output);
	free_output_list(redi->heredoc);
	free(redi);
}

void	free_cmd(t_cmd *cmd)
{
	if (!cmd)
		return;
	free(cmd->args);
	free(cmd->input);
	free(cmd->output);
	free(cmd->heredoc);
	free_output_list(cmd->output2);
	free(cmd);
}

void	free_ast(t_ast *ast)
{
	if (!ast)
		return;
	free_ast(ast->left);
	free_ast(ast->right);
	if (ast->cmd)
		free_cmd(ast->cmd);
	if (ast->type == NODE_SUB)
		free_redi(ast->redi);
	free(ast);
}
