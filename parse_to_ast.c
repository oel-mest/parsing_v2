/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_to_ast.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oel-mest <oel-mest@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 22:46:22 by oel-mest          #+#    #+#             */
/*   Updated: 2025/03/19 00:24:37 by oel-mest         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_special_token(t_token *token)
{
	return (token->type == TOKEN_AND ||
			token->type == TOKEN_OR ||
			token->type == TOKEN_PIPE);
}

int	is_redirection_token(t_token *token)
{
	return (token->type == TOKEN_REDIRECT_OUT ||
			token->type == TOKEN_REDIRECT_IN ||
			token->type == TOKEN_APPEND ||
			token->type == TOKEN_HEREDOC);
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

int	handle_redirect_in(t_cmd **cmd, t_token **tokens)
{
	*tokens = (*tokens)->next;
	
	if (*tokens && is_word_or_quote((*tokens)->type))
	{
		free ((*cmd)->input);
		(*cmd)->input = ft_strdup ((*tokens)->value);
		*tokens = (*tokens)->next;
		return (0);
	}
	else if (*tokens == NULL)
	{
		printf("minishell: syntax error near unexpected token `newline\'\n");
		return (1);
	}
	else
	{
		printf("minishell: syntax error near unexpected token `%s\'\n", (*tokens)->value);
		return (1);
	}
}

int	handle_redirect_out(t_cmd **cmd, t_token **tokens)
{
	int	is_append;

	is_append = ((*tokens)->type == TOKEN_APPEND);
	*tokens = (*tokens)->next;
	if (*tokens && is_word_or_quote((*tokens)->type))
	{
		free ((*cmd)->output);
		(*cmd)->output = ft_strdup ((*tokens)->value);
		add_output (&(*cmd)->output2, create_output_node((*tokens)->value));
		(*cmd)->append = is_append;
		*tokens = (*tokens)->next;
		return (0);
	}
	else if (*tokens == NULL)
	{
		printf("minishell: syntax error near unexpected token `newline\'\n");
		return (1);
	}
	else
	{
		printf("minishell: syntax error near unexpected token `%s\'\n", (*tokens)->value);
		return (1);
	}
}

int	handle_heredoc(t_cmd **cmd, t_token **tokens)
{
	*tokens = (*tokens)->next;
	if (*tokens && is_word_or_quote((*tokens)->type))
	{
		free ((*cmd)->heredoc);
		(*cmd)->heredoc = ft_strdup ((*tokens)->value);
		*tokens = (*tokens)->next;
		return (0);
	}
	else if (*tokens == NULL)
	{
		printf("minishell: syntax error near unexpected token `newline\'\n");
		return (1);
	}
	else
	{
		printf("minishell: syntax error near unexpected token `%s\'\n", (*tokens)->value);
		return (1);
	}
	return (0);
}

static int	process_single_token(t_ast *node, t_token **tokens, int inpar)
{
	t_token	*cur = *tokens;

	if (is_word_or_quote(cur->type))
		handle_word_or_quote(&node->cmd, tokens);
	else if (cur->type == TOKEN_REDIRECT_IN)
		return (handle_redirect_in(&node->cmd, tokens));
	else if (cur->type == TOKEN_REDIRECT_OUT || cur->type == TOKEN_APPEND)
		return (handle_redirect_out(&node->cmd, tokens));
	else if (cur->type == TOKEN_HEREDOC)
		return (handle_heredoc(&node->cmd, tokens));
	else if (cur->type == TOKEN_LPAREN)
		return (printf("10minishell: syntax error near unexpected token `%s'\n",
				cur->value), 1);
	else if (cur->type == TOKEN_RPAREN && !inpar)
		return (printf("11minishell: syntax error near unexpected token `%s'\n",
				cur->value), 1);
	else
		return (2);
	return (0);
}

static int	process_command_tokens(t_ast *node, t_token **tokens, int inpar)
{
	int	ret;

	while (*tokens)
	{
		ret = process_single_token(node, tokens, inpar);
		if (ret == 1)
			return (1);
		if (ret == 2)
			break ;
	}
	return (0);
}

t_ast	*parse_command(t_token **tokens, int inpar)
{
	t_ast	*node;

	if (*tokens == NULL || is_special_token(*tokens) || (*tokens)->type == TOKEN_RPAREN)
	{
		if (*tokens == NULL)
			printf("4minishell: syntax error near unexpected token `newline'\n");
		else
			printf("5minishell: syntax error near unexpected token `%s'\n",
				(*tokens)->value);
		return (NULL);
	}
	node = create_ast_node(NODE_COMMAND, inpar);
	node->cmd = create_cmd_node();
	if (process_command_tokens(node, tokens, inpar))
	{
		free_ast(node);
		return (NULL);
	}
	return (node);
}

static t_ast	*handle_syntax_error(t_ast *left, t_ast *node, t_token **tokens)
{
	if (!tokens || !*tokens)
		printf("6minishell: syntax error near unexpected token `newline'\n");
	else
		printf("7minishell: syntax error near unexpected token `%s'\n",
			(*tokens)->value);
	free_ast(left);
	free_ast(node);
	return (NULL);
}

static t_ast	*parse_pipeline_segment(t_token **tokens, int inpar, t_ast *left)
{
	t_ast	*node;
	t_ast	*right;

	node = create_ast_node(NODE_PIPE, inpar);
	*tokens = (*tokens)->next;
	if (!*tokens)
		return (handle_syntax_error(left, node, tokens));
	if (is_special_token(*tokens))
		return (handle_syntax_error(left, node, tokens));
	if ((*tokens)->type == TOKEN_LPAREN)
		right = handle_parentheses(tokens, inpar);
	else
		right = parse_command(tokens, inpar);
	if (!right)
	{
		free_ast(node);
		return (NULL);
	}
	node->left = left;
	node->right = right;
	return (node);
}

t_ast	*parse_pipeline(t_token **tokens, int inpar)
{
	t_ast	*left;
	t_ast	*node;

	left = parse_command(tokens, inpar);
	if (!left)
		return (NULL);
	while (*tokens && (*tokens)->type == TOKEN_PIPE)
	{
		node = parse_pipeline_segment(tokens, inpar, left);
		if (!node)
			return (NULL);
		left = node;
	}
	return (left);
}
static t_ast	*parse_subshell_content(t_token **tokens, int inpar)
{
	t_ast	*ast;

	ast = create_ast_node(NODE_SUB, inpar);
	ast->left = parse_logical(tokens, 1);
	if (!ast->left)
	{
		free_ast(ast);
		return (NULL);
	}
	return (ast);
}

static t_ast	*handle_parentheses_parent(t_token **tokens, int inpar)
{
	t_ast	*ast;

	*tokens = (*tokens)->next;
	if (!*tokens || (*tokens)->type == TOKEN_RPAREN || is_special_token(*tokens))
	{
		if (!*tokens)
			printf("8minishell: syntax error near unexpected token `newline'\n");
		else
			printf("9minishell: syntax error near unexpected token `%s'\n", (*tokens)->value);
		return (NULL);
	}
	ast = parse_subshell_content(tokens, inpar);
	if (!ast)
		return (NULL);
	if (!*tokens || (*tokens)->type != TOKEN_RPAREN)
	{
		printf("minishell: unclosed parentheses\n");
		free_ast(ast);
		return (NULL);
	}
	*tokens = (*tokens)->next;
	return (ast);
}

static t_ast	*process_redirections(t_token **tokens, t_ast *ast)
{
	while (*tokens && is_redirection_token(*tokens))
	{
		if (parse_redirection(tokens, ast))
			return (NULL);
	}
	return (ast);
}

t_ast	*handle_parentheses(t_token **tokens, int inpar)
{
	t_ast	*ast;

	if (*tokens && (*tokens)->type == TOKEN_LPAREN)
		ast = handle_parentheses_parent(tokens, inpar);
	else if (!*tokens)
	{
		printf("minishell: syntax error\n");
		return (NULL);
	}
	else
		ast = parse_pipeline(tokens, inpar);
	if (!ast)
		return (NULL);
	return (process_redirections(tokens, ast));
}

static t_ast	*process_logical_connector(t_token **tokens, int inpar, t_ast *left)
{
	t_ast		*node;
	t_ast		*right;
	t_node_type	node_type;

	if ((*tokens)->type == TOKEN_AND)
		node_type = NODE_AND;
	else
		node_type = NODE_OR;
	*tokens = (*tokens)->next;
	if (*tokens == NULL || is_special_token(*tokens))
	{
		if (*tokens == NULL)
			printf("minishell: syntax error near unexpected token `newline'\n");
		else
			printf("2minishell: syntax error near unexpected token `%s'\n",
				(*tokens)->value);
		return (free_ast(left) ,NULL);
	}
	node = create_ast_node(node_type, inpar);
	right = handle_parentheses(tokens, inpar);
	if (right == NULL)
		return (free_ast(node), NULL);
	node->left = left;
	node->right = right;
	return (node);
}

static t_ast	*process_logical_pipeline(t_token **tokens, int inpar, t_ast *left)
{
	t_ast	*node;

	node = create_ast_node(NODE_PIPE, inpar);
	node->left = left;
	*tokens = (*tokens)->next;
	node->right = parse_logical(tokens, inpar);
	if (node->right == NULL)
	{
		free_ast(node);
		return (NULL);
	}
	return (node);
}

t_ast	*parse_logical(t_token **tokens, int inpar)
{
	t_ast	*left;
	t_ast	*node;

	left = handle_parentheses(tokens, inpar);
	if (left == NULL)
		return (NULL);
	while (*tokens && ((*tokens)->type == TOKEN_AND || (*tokens)->type == TOKEN_OR))
	{
		node = process_logical_connector(tokens, inpar, left);
		if (node == NULL)
			return (NULL);
		left = node;
	}
	if (*tokens && (*tokens)->type == TOKEN_PIPE)
	{
		node = process_logical_pipeline(tokens, inpar, left);
		if (node == NULL)
			return (NULL);
		left = node;
	}
	return (left);
}

t_ast	*parse(t_token *tokens)
{
	t_ast	*result = parse_logical(&tokens, 0);
	if (result == NULL)
		return (NULL);
	if (tokens != NULL)
	{
		printf("3minishell: syntax error near unexpected token `%s\'\n", tokens->value);
		free_ast(result);
		return (NULL);
	}
	return (result);
}
