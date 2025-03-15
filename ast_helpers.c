/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_helpers.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oel-mest <oel-mest@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 22:45:00 by oel-mest          #+#    #+#             */
/*   Updated: 2025/02/19 15:30:49 by oel-mest         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_ast *create_ast_node(t_node_type type, int inpar)
{
    t_ast *node;

    node = malloc(sizeof(t_ast));
    node->type = type;
    node->cmd = NULL;
    node->left = NULL;
    node->right = NULL;
    node->inpar = inpar;
    if (type == NODE_SUB)
    {
        node->redi = (t_redi *)malloc(sizeof(t_redi));
        node->redi->input = NULL;
        node->redi->output = NULL;
        node->redi->heredoc = NULL;
        node->redi->append = 0;
    }
    return node;
}

t_cmd *create_cmd_node()
{
    t_cmd *cmd;

    cmd = malloc(sizeof(t_cmd));
    cmd->args = NULL;
    cmd->input = NULL;
    cmd->output = NULL;
    cmd->output2 = NULL;
    cmd->append = 0;
    cmd->heredoc = NULL;
    return cmd;
}

void add_argument(t_cmd *cmd, const t_token *token)
{
    char    *tmp;
    char    *tmps;

    if (cmd->args == NULL)
        cmd->args = ft_strdup(token->value);
    else
    {
        if (token->has_space)
        {
            tmp = cmd->args;
            tmps = ft_strjoin(tmp, " ");
            cmd->args = ft_strjoin(tmps, token->value);
            free(tmp);
            free(tmps);
        }
        else
        {
            tmp = cmd->args;
            cmd->args = ft_strjoin(tmp, token->value);
            free(tmp);
        }
    }
}

t_output    *create_output_node(char *file)
{
    t_output    *node;
    
    node = malloc(sizeof(t_output));
    node->file = ft_strdup(file);
    node->next = NULL;
    return (node);
}

void    add_output(t_output **head, t_output *node)
{
    t_output *temp;

    if (!*head)
        *head = node;
    else
    {
        temp = *head;
        while (temp->next)
            temp = temp->next;
        temp->next = node;
    }
}

void    print_outputs(t_output *head)
{
    printf("outputs list files : ");
    while (head)
    {
        printf("%s ", head->file);
        head = head->next;
    }
    printf("\n");
}