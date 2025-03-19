#include "minishell.h"

void append_token(t_token **head, t_token *new_token)
{
    t_token *temp;
    if (!*head)
        *head = new_token;
    else
    {
        temp = *head;
        while (temp->next)
            temp = temp->next;
        temp->next = new_token;
    }
}

static char	*extract_quoted_substr(const char *input, int *i, char quote)
{
	int		start;
	int		length;
	char	*substr;

	start = *i;
	(*i)++;
	while (input[*i] && input[*i] != quote)
		(*i)++;
	if (input[*i] != quote)
	{
		fprintf(stderr, "minishell: syntax error (unclosed quote)\n");
		return (NULL);
	}
	length = *i - start + 1;
	substr = malloc(length + 1);
	if (!substr)
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	strncpy(substr, input + start, length);
	substr[length] = '\0';
	(*i)++;
	return (substr);
}

int	handle_quotes(const char *input, int *i, t_token **tokens, int *has_space)
{
	char	quote;
	char	*substr;
	int		token_type;

	quote = input[*i];
	substr = extract_quoted_substr(input, i, quote);
	if (!substr)
		return (1);
	if (quote == '"')
		token_type = DOUBLE_Q;
	else
		token_type = SINGLE_Q;
	append_token(tokens, create_token(substr, token_type, *has_space));
	free(substr);
	*has_space = 0;
	return (0);
}

static int	is_word_char(char c)
{
	if (!c)
		return (0);
	if (isspace(c))
		return (0);
	if (c == '|' || c == '<' || c == '>' || c == '&' ||
		c == '\'' || c == '"' || c == '(' || c == ')')
		return (0);
	return (1);
}

void    handle_word(const char *input, int *i, t_token **tokens, int *has_space)
{
    int     start;
    int     length;
    char    *word;

    start = *i;
    while (input[*i] && is_word_char(input[*i]))
    {
        (*i)++;
    }
    length = *i - start;
    if (length > 0)
    {
        word = malloc(length + 1);
        if (!word)
        {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        strncpy(word, input + start, length);
        word[length] = '\0';
        append_token(tokens, create_token(word, TOKEN_WORD, *has_space));
        free(word);
        *has_space = 0;
    }
}

void    handle_token_parentheses(const char *input, int *i, t_token **tokens, int *has_space)
{
    char    *token_value;
    int     token_type;

    token_value = malloc(2);
    if (!token_value)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    token_value[0] = input[*i];
    token_value[1] = '\0';
    if (input[*i] == '(')
        token_type = TOKEN_LPAREN;
    else
        token_type = TOKEN_RPAREN;
    append_token(tokens, create_token(token_value, token_type, *has_space));
    free(token_value);
    (*i)++;
    *has_space = 0;
}

static char	*handle_double_token(const char *input, int *i)
{
	char	*token_value;

	token_value = malloc(3);
	if (!token_value)
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	token_value[0] = input[*i];
	token_value[1] = input[*i];
	token_value[2] = '\0';
	*i += 2;
	return (token_value);
}

static char	*handle_single_token(const char *input, int *i)
{
	char	*token_value;

	token_value = malloc(2);
	if (!token_value)
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	token_value[0] = input[*i];
	token_value[1] = '\0';
	(*i)++;
	return (token_value);
}

void	handle_pipes_and_logicals(const char *input, int *i,
			t_token **tokens, int *has_space)
{
	char	*token_value;
	int		token_type;

	if (input[*i + 1] == input[*i])
	{
		token_value = handle_double_token(input, i);
		if (input[*i - 2] == '|')
			token_type = TOKEN_OR;
		else
			token_type = TOKEN_AND;
	}
	else
	{
		token_value = handle_single_token(input, i);
		token_type = TOKEN_PIPE;
	}
	append_token(tokens, create_token(token_value, token_type, *has_space));
	free(token_value);
	*has_space = 0;
}

static char	*get_double_redir(const char *input, int *i, int *token_type)
{
	char	*token_value;

	token_value = malloc(3);
	if (!token_value)
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	token_value[0] = input[*i];
	token_value[1] = input[*i];
	token_value[2] = '\0';
	if (input[*i] == '<')
		*token_type = TOKEN_HEREDOC;
	else
		*token_type = TOKEN_APPEND;
	*i += 2;
	return (token_value);
}

static char	*get_single_redir(const char *input, int *i, int *token_type)
{
	char	*token_value;

	token_value = malloc(2);
	if (!token_value)
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	token_value[0] = input[*i];
	token_value[1] = '\0';
	if (input[*i] == '<')
		*token_type = TOKEN_REDIRECT_IN;
	else
		*token_type = TOKEN_REDIRECT_OUT;
	(*i)++;
	return (token_value);
}

void	handle_redirections(const char *input, int *i,
			t_token **tokens, int *has_space)
{
	char	*token_value;
	int		token_type;

	if (input[*i + 1] == input[*i])
	{
		token_value = get_double_redir(input, i, &token_type);
		append_token(tokens, create_token(token_value, token_type, *has_space));
		free(token_value);
	}
	else
	{
		token_value = get_single_redir(input, i, &token_type);
		append_token(tokens, create_token(token_value, token_type, *has_space));
		free(token_value);
	}
	*has_space = 0;
}

void handle_special_tokens(const char *input, int *i, t_token **tokens, int *has_space)
{
    if (input[*i] == '(' || input[*i] == ')')
        handle_token_parentheses(input, i, tokens, has_space);
    else if (input[*i] == '|' || input[*i] == '&')
        handle_pipes_and_logicals(input, i, tokens, has_space);
    else if (input[*i] == '<' || input[*i] == '>')
        handle_redirections(input, i, tokens, has_space);
}

static void	skip_spaces(const char *input, int *i, int *has_space)
{
	while (input[*i] && isspace(input[*i]))
	{
		*has_space = 1;
		(*i)++;
	}
}

static int	process_token(const char *input, int *i,
				t_token **tokens, int *has_space)
{
	if (input[*i] == '\'' || input[*i] == '"')
	{
		if (handle_quotes(input, i, tokens, has_space))
			return (1);
	}
	else if (input[*i] == '(' || input[*i] == ')' ||
		input[*i] == '|' || input[*i] == '&' ||
		input[*i] == '<' || input[*i] == '>')
	{
		handle_special_tokens(input, i, tokens, has_space);
	}
	else
		handle_word(input, i, tokens, has_space);
	return (0);
}

t_token	*tokenize(const char *input)
{
	t_token	*tokens;
	int		i;
	int		has_space;

	tokens = NULL;
	i = 0;
	has_space = 0;
	while (input[i])
	{
		skip_spaces(input, &i, &has_space);
		if (!input[i])
			break ;
		if (process_token(input, &i, &tokens, &has_space))
		{
			free_tokens(tokens);
			return (NULL);
		}
	}
	return (tokens);
}
