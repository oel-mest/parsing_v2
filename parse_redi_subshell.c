#include "minishell.h"

static int	parse_output_redirection(t_token **tokens, t_redi *redi)
{
	*tokens = (*tokens)->next;
	if (!*tokens || (*tokens)->type != TOKEN_WORD)
	{
		printf("minishell: syntax error expected filename after '>'\n");
		return (1);
	}
	add_output(&redi->output, create_output_node((*tokens)->value));
	*tokens = (*tokens)->next;
	return (0);
}

static int	parse_append_redirection(t_token **tokens, t_redi *redi)
{
	*tokens = (*tokens)->next;
	if (!*tokens || !is_word_or_quote((*tokens)->type))
	{
		printf("minishell: syntax error expected filename after '>>'\n");
		return (1);
	}
	add_output(&redi->output, create_output_node((*tokens)->value));
	redi->append = 1;
	*tokens = (*tokens)->next;
	return (0);
}

static int	parse_input_redirection(t_token **tokens, t_redi *redi)
{
	*tokens = (*tokens)->next;
	if (!*tokens || !is_word_or_quote((*tokens)->type))
	{
		printf("minishell: syntax error expected filename after '<'\n");
		return (1);
	}
	add_output(&redi->input, create_output_node((*tokens)->value));
	*tokens = (*tokens)->next;
	return (0);
}

static int	parse_heredoc_redirection(t_token **tokens, t_redi *redi)
{
	*tokens = (*tokens)->next;
	if (!*tokens || !is_word_or_quote((*tokens)->type))
	{
		printf("minishell: syntax error expected delimiter after '<<'\n");
		return (1);
	}
	add_output(&redi->heredoc, create_output_node((*tokens)->value));
	*tokens = (*tokens)->next;
	return (0);
}

int	parse_single_redirection(t_token **tokens, t_redi *redi)
{
	t_token *token;

	token = *tokens;
	if (token->type == TOKEN_REDIRECT_OUT)
		return (parse_output_redirection(tokens, redi));
	else if (token->type == TOKEN_APPEND)
		return (parse_append_redirection(tokens, redi));
	else if (token->type == TOKEN_REDIRECT_IN)
		return (parse_input_redirection(tokens, redi));
	else if (token->type == TOKEN_HEREDOC)
		return (parse_heredoc_redirection(tokens, redi));
	else
	{
		printf("syntax error: unexpected token in redirection\n");
		return (1);
	}
}


int parse_redirection(t_token **tokens, t_ast *ast)
{
	while (*tokens && is_redirection_token(*tokens))
	{
		if (parse_single_redirection(tokens, ast->redi))
		{
			free_ast(ast);
			return (1);
		}
	}
	return (0);
}

