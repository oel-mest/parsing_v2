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

char    *ft_getenv(char *name, EnvNode *head)
{
    char *value;

    value = NULL;
    while (head != NULL)
    {
        if (strcmp(name + 1, head->var) == 0)
        {
            value = ft_strdup(head->value);
            break;
        }
        head = head->next;
    }
    if (value == NULL)
        return (ft_strdup(""));
    else
        return value;
}

char *append_char(char *str, char c)
{
    char    mystr[2];
    char *new_str;

    mystr[0] = c;
    mystr[1] = 0;
    new_str = ft_strjoin(str, mystr);
    free(str);
    return (new_str);
}

char    *handle_lstatus_var()
{
    return (ft_strdup("0"));
}

char *handle_var_expansion(char *args, int *i, EnvNode *env_list)
{
    char *result;
    int start;
    char *var_name;

    result = NULL;
    if (args[*i] == '?')
    {
        (*i)++;
        result = handle_lstatus_var();
    }
    else if (isalnum(args[*i]) || args[*i] == '_')
    {
        start = *i - 1;
        while (args[*i] && (isalnum(args[*i]) || args[*i] == '_'))
            (*i)++;
        var_name = strndup(args + start, *i - start);
        result = ft_getenv(var_name, env_list);
        free(var_name);
    }
    else
        result = ft_strdup("$");
    return result;
}

static void	update_quotes(char c, int *in_single_quote, int *in_double_quote)
{
	if (c == '\'' && !(*in_double_quote))
		*in_single_quote = !(*in_single_quote);
	if (c == '"' && !(*in_single_quote))
		*in_double_quote = !(*in_double_quote);
}

static char	*process_dollar(const char *args, int *i, char *result,
								EnvNode *env_list)
{
	char	*var_value;
	char	*new_result;

	(*i)++;
	var_value = handle_var_expansion((char *)args, i, env_list);
	if (var_value)
	{
	    new_result = ft_strjoin(result, var_value);
		free(result);
		return (new_result);
	}
	return (result);
}

char	*expand_env_vars(char *args, EnvNode *env_list)
{
	char	*result;
	int		i;
	int		in_single_quote;
	int		in_double_quote;

	result = malloc(1);
	i = 0;
	in_single_quote = 0;
	in_double_quote = 0;
	if (!args || !result)
		return (free(result), NULL);
	result[0] = '\0';
	while (args[i])
	{
		update_quotes(args[i], &in_single_quote, &in_double_quote);
		if (args[i] == '$' && !in_single_quote)
			result = process_dollar(args, &i, result, env_list);
		else
			result = append_char(result, args[i++]);
	}
	return (result);
}
