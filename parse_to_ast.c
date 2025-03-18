/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_to_ast.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oel-mest <oel-mest@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 22:46:22 by oel-mest          #+#    #+#             */
/*   Updated: 2025/03/17 23:46:07 by oel-mest         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_special_token(t_token *token)
{
	return (token->type == TOKEN_AND ||   // '>'
			token->type == TOKEN_OR ||      // '<'
			token->type == TOKEN_PIPE);     // '<<'
}

int	is_redirection_token(t_token *token)
{
	return (token->type == TOKEN_REDIRECT_OUT ||   // '>'
			token->type == TOKEN_REDIRECT_IN ||      // '<'
			token->type == TOKEN_APPEND ||  // '>>'
			token->type == TOKEN_HEREDOC);     // '<<'
}

int	is_word_or_quote(t_token_type type)
{
	return (type == TOKEN_WORD || type == SINGLE_Q || type == DOUBLE_Q);
}

t_ast	*parse_command(t_token **tokens, int inpar)
{
	t_ast	*node;

	if (*tokens == NULL || is_special_token(*tokens))
	{
		if (*tokens == NULL)
			printf("minishell: syntax error near unexpected token `newline\'\n");
		else
			printf("minishell: syntax error near unexpected token `%s\'\n", (*tokens)->value);
		return (NULL);
	}
	node = create_ast_node (NODE_COMMAND, inpar);
	node->cmd = create_cmd_node ();
	while (*tokens)
	{
		if (is_word_or_quote((*tokens)->type))
			handle_word_or_quote(&node->cmd, tokens);
		else if ((*tokens)->type == TOKEN_REDIRECT_IN)
		{
			if (handle_redirect_in(&node->cmd, tokens))
			{
				free_ast(node);
				return (NULL);
			}
		}
		else if ((*tokens)->type == TOKEN_REDIRECT_OUT
			|| (*tokens)->type == TOKEN_APPEND)
		{
			if (handle_redirect_out(&node->cmd, tokens))
			{
				free_ast(node);
				return (NULL);
			}
		}
		else if ((*tokens)->type == TOKEN_HEREDOC)
		{
			if (handle_heredoc(&node->cmd, tokens))
			{
				free_ast(node);
				return (NULL);
			}
		}
		else if ((*tokens)->type == TOKEN_LPAREN)
		{
			printf("minishell:ff syntax error near unexpected token `%s\'\n", (*tokens)->value);
			free_ast(node);
			return (NULL);
		}
		else if ((*tokens)->type == TOKEN_RPAREN && !inpar)
		{
			printf("minishell: syntax error near unexpected token `%s\'\n", (*tokens)->value);
			free_ast(node);
			return (NULL);
		}
		else
			break ;
	}
	return (node);
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

t_ast	*parse_pipeline(t_token **tokens, int inpar)
{
	t_ast	*left;
	t_ast	*node;
	t_ast	*right;

	left = parse_command (tokens, inpar);
	if (left == NULL)
		return (NULL);
	while (*tokens && (*tokens)->type == TOKEN_PIPE)
	{
		node = create_ast_node (NODE_PIPE, inpar);
		*tokens = (*tokens)->next;
		if (*tokens == NULL)
		{
			printf("minishell: syntax error near unexpected token `newline\'\n");
			free_ast(left);
			free_ast(node);
			return (NULL);
		}
		if (is_special_token(*tokens))
		{
			printf("minishell: syntax error near unexpected token `%s\'\n", (*tokens)->value);
			free_ast(left);
			free_ast(node);
			return (NULL);
		}
		if ((*tokens)->type == TOKEN_LPAREN)
			right = handle_parentheses(tokens, inpar);
		else
			right = parse_command (tokens, inpar);
		if (right == NULL)
			return (right);
		node->left = left;
		node->right = right;
		left = node;
	}
	return (left);
}

t_ast	*handle_parentheses(t_token **tokens, int inpar)
{
	t_ast	*ast;

	if (*tokens && (*tokens)->type == TOKEN_LPAREN)
	{
		*tokens = (*tokens)->next;  // Move past the '('
		if (!*tokens || (*tokens)->type == TOKEN_RPAREN || is_special_token(*tokens))
		{
			if (*tokens == NULL)
				printf("minishell: syntax error near unexpected token `newline\'\n");
			else
				printf("minishell: syntax error near unexpected token `%s\'\n", (*tokens)->value);
			return (NULL);
		}
		ast = create_ast_node(NODE_SUB, inpar);  // Create a subshell node
		ast->left = parse_logical(tokens, 1);  // Parse the contents of the parentheses
		if (ast->left == NULL)
		{
			free_ast(ast);
			return (NULL);
		}
		if (!*tokens || (*tokens)->type != TOKEN_RPAREN)
		{
			printf("minishell: unclosed parentheses\n");
			free_ast(ast);
			return (NULL);
		}
		*tokens = (*tokens)->next;  // Move past the ')'
	}
	else if (*tokens == NULL)
	{
		printf("minishell: syntax error\n");
		return (NULL);
	}
	else
	{
		ast = parse_pipeline(tokens, inpar);
		if (ast == NULL)
			return (NULL);
	}

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
	if (left == NULL)
		return (NULL);
	while (*tokens && ((*tokens)->type == TOKEN_AND || (*tokens)->type == TOKEN_OR))
	{
		if ((*tokens)->type == TOKEN_AND)
			node_type = NODE_AND;
		else
			node_type = NODE_OR;
		*tokens = (*tokens)->next;
		if (*tokens == NULL || is_special_token(*tokens))
		{
			if (*tokens == NULL)
				printf("minishell: syntax error near unexpected token `newline\'\n");
			else
				printf("minishell: syntax error near unexpected token `%s\'\n", (*tokens)->value);
			free_ast(left);
			return (NULL);
		}
		node = create_ast_node(node_type, inpar);
		right = handle_parentheses(tokens, inpar);
		if (right == NULL)
		{
			free_ast(node);
			return (NULL);
		}
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
		if (node->right == NULL)
		{
			free_ast(node);
			return (NULL);
		}
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
		printf("minishell: syntax error near unexpected token `%s\'\n", tokens->value);
		free_ast(result);
		return (NULL);
	}
	return (result);
}
