/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhaddad <mhaddad@learner.42.tech>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 16:52:47 by mhaddad           #+#    #+#             */
/*   Updated: 2026/02/05 14:20:06 by mhaddad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static size_t	line_len(char *stash)
{
	int		nl;
	size_t	len;

	nl = ft_strnl(stash);
	if (nl == -1)
	{
		len = ft_strlen(stash);
	}
	else
	{
		len = (size_t)nl + 1;
	}
	return (len);
}

static char	*extract(char *stash)
{
	char	*line;
	size_t	len;
	size_t	i;

	if (!stash || stash[0] == '\0')
	{
		return (NULL);
	}
	len = line_len(stash);
	line = malloc(len + 1);
	if (!line)
	{
		return (NULL);
	}
	i = 0;
	while (i < len)
	{
		line[i] = stash[i];
		i++;
	}
	line[i] = '\0';
	return (line);
}

static char	*clean(char *stash)
{
	char	*new;
	size_t	i;
	size_t	j;
	size_t	len;
	int		nl;

	if (!stash)
		return (NULL);
	nl = ft_strnl(stash);
	if (nl == -1 || stash[nl + 1] == '\0')
		return (free(stash), NULL);
	i = (size_t)nl + 1;
	len = ft_strlen(stash + i);
	new = malloc(len + 1);
	if (!new)
		return (free(stash), NULL);
	j = 0;
	while (stash[i])
		new[j++] = stash[i++];
	new[j] = '\0';
	free(stash);
	return (new);
}

static int	fill(int fd, char **stash)
{
	char	*buf;
	ssize_t	bytes;

	bytes = 0;
	buf = malloc((size_t)BUFFER_SIZE + 1);
	if (!buf)
		return (-1);
	while (ft_strnl(*stash) == -1)
	{
		bytes = read(fd, buf, BUFFER_SIZE);
		if (bytes <= 0)
			break ;
		buf[bytes] = '\0';
		*stash = ft_strjoin(*stash, buf);
		if (!*stash)
		{
			free(buf);
			return (-1);
		}
	}
	free(buf);
	if (bytes < 0)
		return (-1);
	return (0);
}

char	*get_next_line(int fd)
{
	static char	*stash;
	char		*line;

	if (fd < 0 || BUFFER_SIZE <= 0 || read(fd, 0, 0) < 0)
		return (NULL);
	if (fill(fd, &stash) == -1)
	{
		free (stash);
		stash = NULL;
		return (NULL);
	}
	line = extract(stash);
	if (!line)
	{
		free(stash);
		stash = NULL;
		return (NULL);
	}
	stash = clean(stash);
	return (line);
}
