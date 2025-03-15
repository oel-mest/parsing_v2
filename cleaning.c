/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleaning.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oel-mest <oel-mest@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 22:48:30 by oel-mest          #+#    #+#             */
/*   Updated: 2025/03/15 01:15:09 by oel-mest         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_env_list(EnvNode *env_list)
{
	while (env_list)
	{
		EnvNode *next = env_list->next; // Save the next node before freeing the current one
		free(env_list->var);           // Free the variable name string
		free(env_list->value);         // Free the variable value string
		free(env_list);                // Free the current node
		env_list = next;               // Move to the next node
	}
}

void	free_tokens(t_token *tokens)
{
	while (tokens)
	{
		t_token *next = tokens->next; // Save the next node before freeing the current one
		free(tokens->value);         // Free the dynamically allocated string
		free(tokens);                // Free the current token node
		tokens = next;               // Move to the next node
	}
}

void	free_output_list(t_output *output_list)
{
	while (output_list)
	{
		t_output *next = output_list->next;
		if (output_list->file != NULL)
			free(output_list->file); // Free the filename if it exists
		if (output_list != NULL)
			free(output_list);           // Free the node itself
		output_list = next;
	}
}

void	free_redi(t_redi *redi)
{
	if (!redi)
		return;

	// Free the input, output, and heredoc linked lists (safe to pass NULL)
	free_output_list(redi->input);
	free_output_list(redi->output);
	free_output_list(redi->heredoc);

	// Free the t_redi structure itself
	free(redi);
}

void	free_cmd(t_cmd *cmd)
{
	if (!cmd)
		return;

	// Free dynamically allocated strings
	free(cmd->args);
	free(cmd->input);
	free(cmd->output);
	free(cmd->heredoc);

	// Free the output2 linked list
	free_output_list(cmd->output2);

	// Free the t_cmd structure itself
	free(cmd);
}

void	free_ast(t_ast *ast)
{
	if (!ast)
		return;

	// Free the left and right subtrees
	free_ast(ast->left);
	free_ast(ast->right);

	// Free the t_cmd structure (if it exists)
	if (ast->cmd)
		free_cmd(ast->cmd);

	// Free the t_redi structure (if it exists)
	if (ast->type == NODE_SUB)
		free_redi(ast->redi);

	// Free the t_ast structure itself
	free(ast);
}