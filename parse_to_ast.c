/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_to_ast.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oel-mest <oel-mest@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 22:46:22 by oel-mest          #+#    #+#             */
/*   Updated: 2025/02/19 15:23:35 by oel-mest         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_redirection_token(t_token *token)
{
	return (token->type == TOKEN_REDIRECT_OUT ||   // '>'
			token->type == TOKEN_REDIRECT_IN ||      // '<'
			token->type == TOKEN_APPEND ||  // '>>'
			token->type == TOKEN_HEREDOC);     // '<<'
}

t_ast	*parse_command(t_token **tokens, int inpar)
{
	t_ast	*node;

	node = create_ast_node (NODE_COMMAND, inpar);
	node->cmd = create_cmd_node ();
	while (*tokens)
	{
		if (is_word_or_quote((*tokens)->type))
			handle_word_or_quote(&node->cmd, tokens);
		else if ((*tokens)->type == TOKEN_REDIRECT_IN)
			handle_redirect_in(&node->cmd, tokens);
		else if ((*tokens)->type == TOKEN_REDIRECT_OUT
			|| (*tokens)->type == TOKEN_APPEND)
			handle_redirect_out(&node->cmd, tokens);
		else if ((*tokens)->type == TOKEN_HEREDOC)
			handle_heredoc(&node->cmd, tokens);
		else if ((*tokens)->type == TOKEN_LPAREN)
		{
			printf("minishell: syntax error near unexpected token `%s\'\n", (*tokens)->value);
			exit(EXIT_FAILURE);
		}
		else if ((*tokens)->type == TOKEN_RPAREN && !inpar)
		{
			printf("minishell: syntax error near unexpected token `%s\'\n", (*tokens)->value);
			exit(EXIT_FAILURE);
		}
		else
			break ;
	}
	return (node);
}

int	is_word_or_quote(t_token_type type)
{
	return (type == TOKEN_WORD || type == SINGLE_Q || type == DOUBLE_Q);
}

void	handle_word_or_quote(t_cmd **cmd, t_token **tokens)
{
	add_argument ((*cmd), *tokens);
	*tokens = (*tokens)->next;
}

void	handle_redirect_in(t_cmd **cmd, t_token **tokens)
{
	*tokens = (*tokens)->next;
	if (*tokens && (*tokens)->type == TOKEN_WORD)
	{
		free ((*cmd)->input);
		(*cmd)->input = ft_strdup ((*tokens)->value);
		*tokens = (*tokens)->next;
	}
}

void	handle_redirect_out(t_cmd **cmd, t_token **tokens)
{
	int	is_append;

	is_append = ((*tokens)->type == TOKEN_APPEND);
	*tokens = (*tokens)->next;
	if (*tokens && (*tokens)->type == TOKEN_WORD)
	{
		free ((*cmd)->output);
		(*cmd)->output = ft_strdup ((*tokens)->value);
		add_output (&(*cmd)->output2, create_output_node((*tokens)->value));
		(*cmd)->append = is_append;
		*tokens = (*tokens)->next;
	}
}

void	handle_heredoc(t_cmd **cmd, t_token **tokens)
{
	*tokens = (*tokens)->next;
	if (*tokens && ((*tokens)->type == TOKEN_WORD || (*tokens)->type == DOUBLE_Q || (*tokens)->type == SINGLE_Q))
	{
		free ((*cmd)->heredoc);
		(*cmd)->heredoc = ft_strdup ((*tokens)->value);
		*tokens = (*tokens)->next;
	}
	else if (*tokens == NULL)
	{
		printf("minishell: syntax error near unexpected token `newline\'\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("minishell: syntax error near unexpected token `%s\'\n", (*tokens)->value);
		exit(EXIT_FAILURE);
	}
}

t_ast	*parse_pipeline(t_token **tokens, int inpar)
{
	t_ast	*left;
	t_ast	*node;
	t_ast	*right;

	left = parse_command (tokens, inpar);
	while (*tokens && (*tokens)->type == TOKEN_PIPE)
	{
		node = create_ast_node (NODE_PIPE, inpar);
		*tokens = (*tokens)->next;
		if (*tokens == NULL)
		{
			printf("minishell: syntax error\n");
			exit(EXIT_FAILURE);
		}
		right = parse_command (tokens, inpar);
		node->left = left;
		node->right = right;
		left = node;
	}
	return (left);
}

static t_ast	*handle_parentheses(t_token **tokens, int inpar)
{
	t_ast	*ast;

	if (*tokens && (*tokens)->type == TOKEN_LPAREN)
	{
		*tokens = (*tokens)->next;  // Move past the '('
		ast = create_ast_node(NODE_SUB, inpar);  // Create a subshell node
		ast->left = parse_logical(tokens, 1);  // Parse the contents of the parentheses
		if (!*tokens || (*tokens)->type != TOKEN_RPAREN)
		{
			printf("unclosed parentheses\n");
			exit(EXIT_FAILURE);
		}
		*tokens = (*tokens)->next;  // Move past the ')'
	}
	else if (*tokens == NULL)
	{
		printf("minishell: syntax error\n");
		exit(EXIT_FAILURE);
	}
	else
		ast = parse_pipeline(tokens, inpar);  // Handle non-parentheses cases

	while (*tokens && is_redirection_token(*tokens))
	{
		parse_redirection(tokens, ast);
	}
	return (ast);
}

t_ast	*parse_logical(t_token **tokens, int inpar)
{
	t_ast	*left;
	t_ast	*node;
	t_ast	*right;
	t_node_type node_type;

	left = handle_parentheses(tokens, inpar);
	while (*tokens && ((*tokens)->type == TOKEN_AND || (*tokens)->type == TOKEN_OR))
	{
		if ((*tokens)->type == TOKEN_AND)
			node_type = NODE_AND;
		else
			node_type = NODE_OR;
		node = create_ast_node(node_type, inpar);
		*tokens = (*tokens)->next;
		right = handle_parentheses(tokens, inpar);
		node->left = left;
		node->right = right;
		left = node;
	}
	if (*tokens && (*tokens)->type == TOKEN_PIPE)
	{
		node = create_ast_node(NODE_PIPE, inpar);
		node->left = left;
		*tokens = (*tokens)->next;
		node->right = parse_pipeline(tokens, inpar);
		left = node;
	}
	return (left);
}

t_ast	*parse(t_token *tokens)
{
	t_ast	*result = parse_logical(&tokens, 0);
	if (tokens != NULL)
	{
		printf("minishell: syntax error xx near unexpected token `%s\'\n", tokens->value);
		exit(EXIT_FAILURE);
	}
	return (result);
}
