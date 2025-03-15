#include "minishell.h"

void    ft_exit_f(char *str)
{
    printf("%s\n", str);
    exit(EXIT_FAILURE);
}

EnvNode *create_node(const char *env_entry)
{
    EnvNode *node;
    char *eq;
    size_t name_len;

    node = malloc(sizeof(EnvNode));
    if (!node)
        ft_exit_f("malloc error");
    eq = strchr(env_entry, '=');
    if (eq == NULL)
    {
        node->var = strdup(env_entry);
        node->value = strdup("");
    }
    else
    {
        name_len = eq - env_entry;
        node->var = malloc(name_len + 1);
        if (!node->var)
            ft_exit_f("malloc error");
        strncpy(node->var, env_entry, name_len);
        node->var[name_len] = '\0';
        node->value = strdup(eq + 1);
    }
    node->next = NULL;
    return node;
}

void add_node(EnvNode **head, EnvNode *new_node)
{
    EnvNode *current;

    if (*head == NULL)
        *head = new_node;
    else
    {
        current = *head;
        while (current->next != NULL)
            current = current->next;
        current->next = new_node;
    }
}

void free_list(EnvNode *head)
{
    while (head != NULL) {
        EnvNode *temp = head;
        head = head->next;
        free(temp->var);
        free(temp->value);
        free(temp);
    }
}

int is_expandable(char *str)
{
    while (str && *str)
    {
        if (*str == '$')
            return 1;
        str++;
    }
    return 0;
}

char    *ft_getenv(char *name, EnvNode *head) // '$HOME'
{
    char *value;
    
    value = NULL;
    while (head != NULL)
    {
        if (strcmp(name + 1, head->var) == 0)
        {
            value = strdup(head->value);
            break;
        }
        head = head->next;
    }
    if (value == NULL)
        return (strdup(""));
    else
        return value;
}  

void ft_expand(t_token *token, EnvNode *head)
{
    char *input = token->value;
    size_t out_size = 128;
    size_t out_index = 0;
    char *output = malloc(out_size);
    if (!output)
        ft_exit_f("malloc error");
    for (size_t i = 0; input[i] != '\0'; )
    {
        if (input[i] == '$' && (isalpha(input[i + 1]) || input[i + 1] == '_'))
        {
            size_t name_start = i + 1;
            i++;
            while (isalnum(input[i]) || input[i] == '_')
                i++;
            size_t name_len = i - name_start;
            char *var_str = malloc(name_len + 2);
            if (!var_str) {
                perror("malloc");
                exit(EXIT_FAILURE);
            }
            var_str[0] = '$';
            strncpy(var_str + 1, &input[name_start], name_len);
            var_str[name_len + 1] = '\0';
            char *env_val = ft_getenv(var_str, head);
            free(var_str);
            size_t val_len = strlen(env_val);
            if (out_index + val_len + 1 > out_size) {
                out_size = (out_index + val_len + 1) * 2;
                output = realloc(output, out_size);
                if (!output) {
                    perror("realloc");
                    exit(EXIT_FAILURE);
                }
            }
            memcpy(output + out_index, env_val, val_len);
            out_index += val_len;
        } else {
            if (out_index + 2 > out_size) {
                out_size *= 2;
                output = realloc(output, out_size);
                if (!output) {
                    perror("realloc");
                    exit(EXIT_FAILURE);
                }
            }
            output[out_index++] = input[i++];
        }
    }
    output[out_index] = '\0';
    free(token->value);
    token->value = output;
}

char *append_char(char *str, char c)
{
    char *new_str = ft_strjoin(str, (char[]){c, '\0'});
    free(str);
    return new_str;
}

char    *handle_lstatus_var()
{
    return (ft_strdup("0"));
}

// Helper function to handle variable expansion
char *handle_var_expansion(char *args, int *i, EnvNode *env_list)
{
    char *result = NULL;
    if (args[*i] == '?')
    {
        (*i)++;
        result = handle_lstatus_var();
    }
    else if (isalnum(args[*i]) || args[*i] == '_')
    {
        int start = *i - 1;
        while (args[*i] && (isalnum(args[*i]) || args[*i] == '_'))
            (*i)++;
        char *var_name = strndup(args + start, *i - start);
        result = ft_getenv(var_name, env_list);
        free(var_name);
    }
    else
        result = ft_strdup("$");
    return result;
}

// Main function to expand environment variables
char *expand_env_vars(char *args, EnvNode *env_list)
{
    char *result = malloc(1);
    int i = 0;
    int in_single_quote = 0;
    int in_double_quote = 0;

    if (!args || !result)
        return (free(result), NULL);
    result[0] = '\0';

    while (args[i])
    {
        if (args[i] == '\'' && !in_double_quote)
            in_single_quote = !in_single_quote;
        if (args[i] == '"' && !in_single_quote)
            in_double_quote = !in_double_quote;
        if (args[i] == '$' && !in_single_quote)
        {
            i++;
            char *var_value = handle_var_expansion(args, &i, env_list);
            if (var_value)
            {
                char *new_result = ft_strjoin(result, var_value);
                free(result);
                result = new_result;
            }
        }
        else
            result = append_char(result, args[i++]);
    }
    return result;
}

void    expand_tokens(t_token *tokens, EnvNode *head)
{
    while (tokens != NULL)
    {
        if (tokens->type == TOKEN_WORD || tokens->type == DOUBLE_Q)
        {
            if (is_expandable(tokens->value))
                ft_expand(tokens, head);
        }
        tokens = tokens->next;
    }
}
