/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleaning.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oel-mest <oel-mest@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 22:48:30 by oel-mest          #+#    #+#             */
/*   Updated: 2025/02/18 22:48:48 by oel-mest         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void free_cmd(t_cmd *cmd)
{
    if (!cmd) return;
    free(cmd->args);
    free(cmd->input);
    free(cmd->output);
    free(cmd->heredoc);
    free(cmd);
}

void free_ast(t_ast *node)
{
    if (!node) return;

    free_ast(node->left);
    free_ast(node->right);

    if (node->type == NODE_COMMAND) {
        free_cmd(node->cmd);
    }

    free(node);
}