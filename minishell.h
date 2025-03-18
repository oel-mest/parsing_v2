/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oel-mest <oel-mest@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 21:59:27 by oel-mest          #+#    #+#             */
/*   Updated: 2025/03/18 00:01:42 by oel-mest         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
#define MINISHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <readline/readline.h>
#include <readline/history.h>

typedef struct s_output
{
    char *file;
    struct s_output *next;
}   t_output;

typedef enum e_token_type {
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_WORD,
	SINGLE_Q,
	DOUBLE_Q,
    TOKEN_PIPE,         // |
    TOKEN_REDIRECT_IN,  // <
    TOKEN_REDIRECT_OUT, // >
    TOKEN_APPEND,       // >>
    TOKEN_HEREDOC,      // <<
    TOKEN_AND,          // &&
    TOKEN_OR            // ||
} t_token_type;

typedef enum e_node_type {
    NODE_COMMAND,  // A single command with args (e.g., "echo hello")
    NODE_PIPE,     // A pipeline operator (|)
    NODE_AND,      // Logical AND (&&)
    NODE_OR,        // Logical OR (||)
    NODE_SUB
} t_node_type;

typedef struct s_redi {
    t_output *input;
    t_output *output;
    t_output *heredoc;
    int append;
}   t_redi;

typedef struct s_token {
    char *value;
    t_token_type type;
    int has_space;
    struct s_token *next;
} t_token;

typedef struct s_cmd {
    char *args;    // Command arguments (string )
    char *input;    // File for input redirection (<)
    char *output;   // File for output redirection (>)
    t_output *output2;
    int append;     // 1 if >> (append), 0 if > (truncate)
    char *heredoc;  // Delimiter for heredoc (<<)
} t_cmd;

typedef struct s_ast {
    t_node_type type;     // COMMAND, PIPE, AND, OR, Subshell
    t_cmd *cmd;           // Only for command nodes
    t_redi *redi;
    struct s_ast *left;   // Left child (for &&, ||, or pipes)
    struct s_ast *right;  // Right child (for &&, ||, or pipes)
    int	inpar;
} t_ast;

typedef struct EnvNode {
    char *var;            // Variable name
    char *value;          // Variable value
    struct EnvNode *next; // Pointer to next node
} EnvNode;

t_ast *create_ast_node(t_node_type type, int inpar);
t_cmd *create_cmd_node();
void add_argument(t_cmd *cmd, const t_token *token);



t_ast *parse_command(t_token **tokens, int inpar);
int	handle_heredoc(t_cmd **cmd, t_token **tokens);
int	handle_redirect_out(t_cmd **cmd, t_token **tokens);
int	handle_redirect_in(t_cmd **cmd, t_token **tokens);
void	handle_word_or_quote(t_cmd **cmd, t_token **tokens);
int	is_word_or_quote(t_token_type type);

t_ast *parse_pipeline(t_token **tokens, int inpar);
t_ast *parse_logical(t_token **tokens, int inpar);
t_ast *parse(t_token *tokens);

void free_cmd(t_cmd *cmd);
void free_ast(t_ast *node);
void	free_tokens(t_token *tokens);
void	free_env_list(EnvNode *env_list);

void print_indentation(int depth, int is_last);
void print_cmd(t_cmd *cmd, int depth, int is_last, EnvNode *head);
void print_ast(t_ast *node, int depth, int is_last, EnvNode *head);
void print_ast_tree(t_ast *root, EnvNode *head);

t_output    *create_output_node(char *file);
void    add_output(t_output **head, t_output *node);
void    print_outputs(t_output *head);

char	*ft_strjoin(const char *s1, const char *s2);
char	*ft_strdup(char *src);
size_t	ft_strlen(const char *str);

char    *ft_getenv(char *name, EnvNode *head);
void add_node(EnvNode **head, EnvNode *new_node);
EnvNode *create_node(const char *env_entry);
void free_list(EnvNode *head);

void    expand_tokens(t_token *tokens, EnvNode *head);
char *expand_env_vars(char *args, EnvNode *env_list);

int parse_redirection(t_token **tokens, t_ast *ast);
int	is_redirection_token(t_token *token);

t_token *create_token(const char *value, t_token_type type, int l_space);
void append_token(t_token **head, t_token *new_token);
t_token *tokenize(const char *input);
t_ast	*handle_parentheses(t_token **tokens, int inpar);

int	is_word_or_quote(t_token_type type);
#endif