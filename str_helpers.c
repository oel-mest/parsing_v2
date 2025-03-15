#include "minishell.h"

size_t	ft_strlen(const char *str)
{
	size_t	i;

	i = 0;
	while (str[i])
	{
		i++;
	}
	return (i);
}

char	*ft_strdup(char *src)
{
	char	*cpy;
	int		len;
	int		i;

	len = 0;
	while (src[len])
		len++;
	cpy = (char *)malloc(len + 1);
	if (cpy == NULL)
		return (NULL);
	i = 0;
	while (src[i] != '\0')
	{
		cpy[i] = src[i];
		i++;
	}
	cpy[i] = '\0';
	return (cpy);
}

char	*ft_strjoin(const char *s1, const char *s2)
{
	char	*new_str;
	int		i;
	int		j;

	if (!s1 && !s2)
		return (NULL);
	if (!s1)
		return (ft_strdup((char *)s2));
	if (!s2)
		return (ft_strdup((char *)s1));
	new_str = (char *)malloc (ft_strlen (s1) + ft_strlen (s2) + 1);
	if (!new_str)
		return (NULL);
	i = -1;
	while (s1[++i] != '\0')
		new_str[i] = s1[i];
	j = 0;
	while (s2[j] != '\0')
		new_str[i++] = s2[j++];
	new_str[i] = '\0';
	return (new_str);
}