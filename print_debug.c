/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_debug.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oel-mest <oel-mest@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 22:52:34 by oel-mest          #+#    #+#             */
/*   Updated: 2025/03/18 02:04:52 by oel-mest         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void print_indentation(int depth, int is_last)
{
    for (int i = 0; i < depth; i++) {
        if (i == depth - 1) {
            printf(is_last ? "└── " : "├── ");
        } else {
            printf("    ");
        }
    }
}

void print_cmd(t_cmd *cmd, int depth, int is_last, EnvNode *head)
{
    print_indentation(depth, is_last);
    printf("COMMAND:\n");

    // Print arguments
    print_indentation(depth + 1, 0);
    printf("ARGS: %s\n", cmd->args);
    print_indentation(depth + 1, 0);
    printf("EX ARGS: %s\n", expand_env_vars(cmd->args, head));

    // Print input redirection
    if (cmd->input) {
        print_indentation(depth + 1, 0);
        printf("INPUT: %s\n", cmd->input);
    }

    // Print output redirection
    if (cmd->output) {
        print_indentation(depth + 1, 0);
        printf("OUTPUT: %s (%s)\n", cmd->output, cmd->append ? "APPEND" : "TRUNCATE");
        print_indentation(depth + 1, 0);
        print_outputs(cmd->output2);
    }

    // Print heredoc
    if (cmd->heredoc) {
        print_indentation(depth + 1, 0);
        printf("HEREDOC: %s\n", cmd->heredoc);
    }
}

void print_ast(t_ast *node, int depth, int is_last, EnvNode *head)
{
    if (!node) return;

    // Print the current node
    print_indentation(depth, is_last);
    switch (node->type) {
        case NODE_COMMAND:
            printf("COMMAND NODE (%d):\n", node->inpar);
            print_cmd(node->cmd, depth + 1, 1, head);
            break;
        case NODE_PIPE:
            printf("PIPE NODE(%d):\n", node->inpar);
            break;
        case NODE_AND:
            printf("AND NODE (&&)(%d):\n", node->inpar);
            break;
        case NODE_OR:
            printf("OR NODE (||)(%d):\n", node->inpar);
            break;
        case NODE_SUB:
            printf("SUBSHELL ()(%d):\n", node->inpar);
            print_indentation(depth, is_last);
            print_outputs(node->redi->output);
            print_indentation(depth, is_last);
            print_outputs(node->redi->heredoc);
            print_indentation(depth, is_last);
            print_outputs(node->redi->input);
            break;
        default:
            printf("UNKNOWN NODE: %d\n", node->type);
            break;
    }

    // Recursively print children
    if (node->left) {
        print_ast(node->left, depth + 1, node->right == NULL, head);
    }
    if (node->right) {
        print_ast(node->right, depth + 1, 1, head);
    }
}

void print_ast_tree(t_ast *root, EnvNode *head)
{
    printf("AST TREE:\n");
    print_ast(root, 0, 1, head);
}