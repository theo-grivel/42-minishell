/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   line_env.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldominiq <ldominiq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/21 16:21:56 by tgrivel           #+#    #+#             */
/*   Updated: 2022/06/29 21:31:03 by melogr@phy       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include	"minishell.h"

// an enviromnent variable [A-Z] or [a-z] or '_'
static int	name_env(char c)
{
	if (c == '_')
		return (1);
	if ('a' <= c && c <= 'z')
		return (1);
	if ('A' <= c && c <= 'Z')
		return (1);
	return (0);
}

// get length of the enviromnent variable
static char	*get_envlen(char *line, char **envp, int *i, int *len)
{
	char	*env;
	char	*var;
	int		start;

	start = *i;
	while (name_env(line[*i]))
	{
		(*i)++;
	}
	var = ft_strndup((line + start), *i - start);
	env = my_getenv(var, envp);
	free(var);
	if (len)
		*len += ft_strlen(env);
	return (env);
}

// ind[0]: index of line
// ind[1]: toggle double quote
// ind[2]: length of the environment variable
static inline void	init_index_get_len(int ind[3])
{
	ind[0] = 0;
	ind[1] = 1;
	ind[2] = 0;
}

// get length of the line if we replace all environment variable
static int	get_len(char *line, char **envp)
{
	int	ind[3];

	init_index_get_len(ind);
	while (line[ind[0]])
	{
		if (line[ind[0]] == '\"' && ++ind[0] && ++ind[2])
			ind[1] = (ind[1] + 1) % 2;
		while (ind[1] && line[ind[0]] == '\'' && ++ind[0])
		{
			ind[2] += 2;
			while (line[ind[0]] && line[ind[0]] != '\'' && ++ind[0])
				ind[2]++;
			if (line[ind[0]] == 0)
				return (-1);
			ind[0]++;
		}
		while (line[ind[0]] == '$' && ++ind[0])
			get_envlen(line, envp, &(ind[0]), &(ind[2]));
		while (line[ind[0]] && line[ind[0]] != '\"' && (line[ind[0]] != '\''
				|| !ind[1]) && line[ind[0]] != '$' && ++ind[0])
			ind[2]++;
	}
	if (ind[1] == 0)
		return (-1);
	return (ind[2]);
}

// While single quote
static inline void	simple_quote(char *ret, char *line, int ind[5])
{
	ret[ind[1]++] = line[ind[0]++];
	while (line[ind[0]] && line[ind[0]] != '\'')
		ret[ind[1]++] = line[ind[0]++];
	ret[ind[1]++] = line[ind[0]++];
}

// While there is dollars in line
static void	dollars(char *ret, char *line, int ind[5], char **envp)
{
	char	*env;

	env = get_envlen(line, envp, &ind[0], 0);
	ind[2] = 0;
	while (env && env[ind[2]])
		ret[ind[1]++] = env[ind[2]++];
}

// ind[0]: index of line
// ind[1]: index of the return line
// ind[2]: index of environment variable
// ind[3]: toggle double quotes
// ind[4]: lenght of the return string
static char	*init_index(int ind[5], char *line, char **envp)
{
	char	*ret;

	ind[0] = 0;
	ind[1] = 0;
	ind[2] = 0;
	ind[3] = 1;
	ind[4] = get_len(line, envp);
	if (ind[4] == -1)
	{
		free(line);
		return (0);
	}
	ret = malloc(ind[4] + 1);
	if (ret == 0)
		return (0);
	return (ret);
}

// return new line where the environment variable are replaced
// env: the string of environment variable
// ret: the returned string
// ind: the table of index, lenght and toggle variable
char	*line_env(char *line, char **envp)
{
	char	*ret;
	int		ind[5];

	ret = init_index(ind, line, envp);
	if (ret == 0)
		return (0);
	ret[ind[4]] = 0;
	while (line[ind[0]])
	{
		if (line[ind[0]] == '\"')
		{
			ret[ind[1]++] = line[ind[0]++];
			ind[3] = (ind[3] + 1) % 2;
		}
		while (line[ind[0]] == '\'' && ind[3])
			simple_quote(ret, line, ind);
		while (line[ind[0]] == '$' && ++ind[0])
			dollars(ret, line, ind, envp);
		while (line[ind[0]] && (line[ind[0]] != '\"' || ind[3])
			&& (line[ind[0]] != '\'' || !ind[3]) && line[ind[0]] != '$')
			ret[ind[1]++] = line[ind[0]++];
	}
	free(line);
	return (ret);
}
