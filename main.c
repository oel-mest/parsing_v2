#include "minishell.h"

char *read_input(void)
{
    char *line = NULL;

	line = readline(">>");
    if (!line) {
        free(line);
        return NULL;
    }
    return (line);
}

t_token *create_token(const char *value, t_token_type type, int l_space)
{
    t_token *token;

    token = malloc(sizeof(t_token));
    if (!token)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    token->value = strdup(value);
    token->type = type;
    token->next = NULL;
    token->has_space = l_space;
    return (token);
}

void print_tokens(t_token *tokens)
{
    while (tokens)
    {
        printf("Token: %-10s Type: %d\tspaceb : %d\n", tokens->value, tokens->type, tokens->has_space);
        tokens = tokens->next;
    }
}

void    check_leaks()
{
    system("leaks a.out");
}

int main(int argc, char *argv[], char *envp[])
{ 
    atexit(check_leaks);
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
        if (tokens != NULL)
        {
            print_tokens(tokens);
            t_ast *ast = parse(tokens);
            free_tokens(tokens);
            free(line);
            if (ast != NULL)
            {
                print_ast_tree(ast, head);
                free_ast(ast);
            }
        }
        //free_env_list(head);
		line = read_input();
	}
    return EXIT_SUCCESS;
}
