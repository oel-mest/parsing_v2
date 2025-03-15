#include "minishell.h"

char *read_input(void)
{
    char *line = NULL;

	line = readline(">>");
    if (!line) {
        free(line);
        return NULL;
    }
    return line;
}

t_token *create_token(const char *value, t_token_type type, int l_space) {
    t_token *token = malloc(sizeof(t_token));
    if (!token) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    token->value = strdup(value);
    token->type = type;
    token->next = NULL;
    token->has_space = l_space;
    return token;
}

void append_token(t_token **head, t_token *new_token) {
    if (!*head) {
        *head = new_token;
    } else {
        t_token *temp = *head;
        while (temp->next)
            temp = temp->next;
        temp->next = new_token;
    }
}

t_token *tokenize(const char *input) {
    t_token *tokens = NULL;
    int i = 0;
    while (input[i]) {
        while (input[i] && isspace(input[i]))
            i++;
        if (!input[i])
            break;
        if (input[i]== '(')
        {
            append_token(&tokens, create_token("(", TOKEN_LPAREN, 0));
            i++;
        }
        else if (input[i]== ')')
        {
            append_token(&tokens, create_token(")", TOKEN_RPAREN, 0));
            i++;
        }
        else if (input[i] == '|') {
            if (input[i + 1] == '|') {
                append_token(&tokens, create_token("||", TOKEN_OR, 0));
                i += 2;
            } else {
                append_token(&tokens, create_token("|", TOKEN_PIPE, 0));
                i++;
            }
        }
        else if (input[i] == '<') {
            if (input[i + 1] == '<') {
                append_token(&tokens, create_token("<<", TOKEN_HEREDOC, 0));
                i += 2;
            } else {
                append_token(&tokens, create_token("<", TOKEN_REDIRECT_IN, 0));
                i++;
            }
        }
        else if (input[i] == '>') {
            if (input[i + 1] == '>') {
                append_token(&tokens, create_token(">>", TOKEN_APPEND, 0));
                i += 2;
            } else {
                append_token(&tokens, create_token(">", TOKEN_REDIRECT_OUT, 0));
                i++;
            }
        }
        else if (input[i] == '&') {
            if (input[i + 1] == '&') {
                append_token(&tokens, create_token("&&", TOKEN_AND, 0));
                i += 2;
            } else {
                i++;
            }
        }
        else if (input[i] == '\'' || input[i] == '"') {
            char quote = input[i];
            int has_space = 0;
            int start = i;
			i++;
            while (input[i] && input[i] != quote )
                i++;
            if (input[i] != quote) {
                fprintf(stderr, "Error: Unclosed quote\n");
                return tokens;
            }
            int length = i - start + 1;
			if (length > 2)
			{
				char *substr = malloc(length + 1);
				if (!substr) {
					perror("malloc");
					exit(EXIT_FAILURE);
				}
				strncpy(substr, input + start, length);
				substr[length] = '\0';
                if (isspace(input[start - 1]))
                    has_space = 1;
				if (quote == '"')
					append_token(&tokens, create_token(substr, DOUBLE_Q, has_space));
				else
					append_token(&tokens, create_token(substr, SINGLE_Q, has_space));
				free(substr);
			}
            i++;
        }
        else {
            int has_space = 0;
            int start = i;
            while (input[i] && !isspace(input[i]) &&
                	input[i] != '|' && input[i] != '<' &&
                	input[i] != '>' && input[i] != '&' &&
                    input[i] != '\'' && input[i] != '"' &&
                    input[i] != '(' && input[i] != ')' )
                i++;
            int length = i - start;
            char *word = malloc(length + 1);
            if (!word) {
                perror("malloc");
                exit(EXIT_FAILURE);
            }
            strncpy(word, input + start, length);
            word[length] = '\0';
            if (isspace(input[start - 1]))
                has_space = 1;
            append_token(&tokens, create_token(word, TOKEN_WORD, has_space));
            free(word);
        }
    }
    return tokens;
}

/* Helper: Free the tokens list */
void free_tokens(t_token *tokens) {
    while (tokens) {
        t_token *tmp = tokens;
        tokens = tokens->next;
        free(tmp->value);
        free(tmp);
    }
}

void print_tokens(t_token *tokens) {
    while (tokens) {
        printf("Token: %-10s Type: %d\tspaceb : %d\n", tokens->value, tokens->type, tokens->has_space);
        tokens = tokens->next;
    }
}

int main(int argc, char *argv[], char *envp[])
{
    EnvNode *head = NULL;

    for (int i = 0; envp[i] != NULL; i++)
    {
        EnvNode *node = create_node(envp[i]);
        add_node(&head, node);
    }
    printf("Minishell by oussama\n");
    char *line = read_input();
	while(line != NULL)
	{
        add_history(line);
		t_token *tokens = tokenize(line);
		print_tokens(tokens);
        //expand_tokens(tokens, head);
        //printf("Expanded version\n");
        //print_tokens(tokens);
		t_ast *ast = parse(tokens);
		free_tokens(tokens);
		free(line);
		print_ast_tree(ast, head);
		free_ast(ast);
		line = read_input();
	}
    return EXIT_SUCCESS;
}
