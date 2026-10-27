#include "get_next_line.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int	main(int argc, char **argv)
{
	int		fd;
	char	*line;

	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s FILE\n", argv[0]);
		return (1);
	}
	fd = open(argv[1], O_RDONLY);
	if (fd < 0)
	{
		perror(argv[1]);
		return (1);
	}
	line = get_next_line(fd);
	while (line)
	{
		fputs(line, stdout);
		free(line);
		line = get_next_line(fd);
	}
	close(fd);
	return (0);
}
