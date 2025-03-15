#include "minishell.h"

void    handle_quotes(const char *input, int *i, t_token **tokens, int *has_space)
{
    char    quote;
    int     start;
    int     length;
    char    *substr;
    int     token_type;

    quote = input[*i];
    start = *i;
    (*i)++;
    while (input[*i] && input[*i] != quote)
        (*i)++;
    if (input[*i] != quote)
    {
        fprintf(stderr, "Error: Unclosed quote\n");
        return;
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
    if (quote == '"')
        token_type = DOUBLE_Q;
    else
        token_type = SINGLE_Q;
    append_token(tokens, create_token(substr, token_type, *has_space));
    free(substr);
    (*i)++;
    *has_space = 0;
}

void    handle_word(const char *input, int *i, t_token **tokens, int *has_space)
{
    int     start;
    int     length;
    char    *word;

    start = *i;
    while (input[*i] && !isspace(input[*i]) &&
           input[*i] != '|' && input[*i] != '<' &&
           input[*i] != '>' && input[*i] != '&' &&
           input[*i] != '\'' && input[*i] != '"' &&
           input[*i] != '(' && input[*i] != ')')
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

void    handle_parentheses(const char *input, int *i, t_token **tokens, int *has_space)
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

void    handle_pipes_and_logicals(const char *input, int *i, t_token **tokens, int *has_space)
{
    char    *token_value;
    int     token_type;

    if (input[*i + 1] == input[*i])
    {
        token_value = malloc(3);
        if (!token_value)
        {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        token_value[0] = input[*i];
        token_value[1] = input[*i];
        token_value[2] = '\0';
        if (input[*i] == '|')
            token_type = TOKEN_OR;
        else
            token_type = TOKEN_AND;
        append_token(tokens, create_token(token_value, token_type, *has_space));
        free(token_value);
        (*i) += 2;
    }
    else
    {
        token_value = malloc(2);
        if (!token_value)
        {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        token_value[0] = input[*i];
        token_value[1] = '\0';
        append_token(tokens, create_token(token_value, TOKEN_PIPE, *has_space));
        free(token_value);
        (*i)++;
    }
    *has_space = 0;
}

void    handle_redirections(const char *input, int *i, t_token **tokens, int *has_space)
{
    char    *token_value;
    int     token_type;

    if (input[*i + 1] == input[*i])
    {
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
            token_type = TOKEN_HEREDOC;
        else
            token_type = TOKEN_APPEND;
        append_token(tokens, create_token(token_value, token_type, *has_space));
        free(token_value);
        (*i) += 2;
    }
    else
    {
        token_value = malloc(2);
        if (!token_value)
        {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        token_value[0] = input[*i];
        token_value[1] = '\0';
        if (input[*i] == '<')
            token_type = TOKEN_REDIRECT_IN;
        else
            token_type = TOKEN_REDIRECT_OUT;
        append_token(tokens, create_token(token_value, token_type, *has_space));
        free(token_value);
        (*i)++;
    }
    *has_space = 0;
}

void handle_special_tokens(const char *input, int *i, t_token **tokens, int *has_space)
{
    if (input[*i] == '(' || input[*i] == ')')
        handle_parentheses(input, i, tokens, has_space);
    else if (input[*i] == '|' || input[*i] == '&')
        handle_pipes_and_logicals(input, i, tokens, has_space);
    else if (input[*i] == '<' || input[*i] == '>')
        handle_redirections(input, i, tokens, has_space);
}

t_token *tokenize(const char *input)
{
    t_token *tokens;
    int     i;
    int     has_space;

    tokens = NULL;
    i = 0;
    has_space = 0;
    while (input[i])
    {
        while (input[i] && isspace(input[i]))
        {
            has_space = 1;
            i++;
        }
        if (!input[i])
            break;
        if (input[i] == '\'' || input[i] == '"')
            handle_quotes(input, &i, &tokens, &has_space);
        else if (input[i] == '(' || input[i] == ')' ||
                 input[i] == '|' || input[i] == '&' ||
                 input[i] == '<' || input[i] == '>')
        {
            handle_special_tokens(input, &i, &tokens, &has_space);
        }
        else
            handle_word(input, &i, &tokens, &has_space);
    }
    return (tokens);
}
