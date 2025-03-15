#include "minishell.h"

void parse_single_redirection(t_token **tokens, t_redi *redi)
{
	t_token *token = *tokens;
	if (token->type == TOKEN_REDIRECT_OUT) // '>'
	{
		*tokens = token->next;
		if (!*tokens || (*tokens)->type != TOKEN_WORD)
		{
			printf("syntax error: expected filename after '>'\n");
			exit(EXIT_FAILURE);
		}
        add_output (&redi->output, create_output_node((*tokens)->value));
		*tokens = (*tokens)->next;
	}
	else if (token->type == TOKEN_APPEND) // '>>'
	{
		*tokens = token->next;
		if (!*tokens || (*tokens)->type != TOKEN_WORD)
		{
			printf("syntax error: expected filename after '>>'\n");
			exit(EXIT_FAILURE);
		}
		add_output (&redi->output, create_output_node((*tokens)->value));
		redi->append = 1; // Set append flag
		*tokens = (*tokens)->next;
	}
	else if (token->type == TOKEN_REDIRECT_IN) // '<'
	{
		*tokens = token->next;
		if (!*tokens || (*tokens)->type != TOKEN_WORD)
		{
			printf("syntax error: expected filename after '<'\n");
			exit(EXIT_FAILURE);
		}
		add_output (&redi->input, create_output_node((*tokens)->value));
		*tokens = (*tokens)->next;
	}
	else if (token->type == TOKEN_HEREDOC) // '<<'
	{
		*tokens = token->next;
		if (!*tokens || (*tokens)->type != TOKEN_WORD)
		{
			printf("syntax error: expected delimiter after '<<'\n");
			exit(EXIT_FAILURE);
		}
		add_output (&redi->heredoc, create_output_node((*tokens)->value));
		*tokens = (*tokens)->next;
	}
	else
	{
		printf("syntax error: unexpected token in redirection\n");
		exit(EXIT_FAILURE);
	}
}

void parse_redirection(t_token **tokens, t_ast *ast)
{
	while (*tokens && is_redirection_token(*tokens))
		parse_single_redirection(tokens, ast->redi);
}

