#include <unistd.h>
#include <stdlib.h>

#ifndef BUFFER_SIZE
# define BUFFER_SIZE 1024
#endif

char
*gnl(int fd)
{
	static char	buff[BUFFER_SIZE];
	static int	buff_offset;
	static int	buff_len;
	char	*line;
	char	*tmp;
	int		line_len;
	int		i;

	if (fd == -1)
		return (NULL);
	line = NULL;
	line_len = 0;
	while (line_len == 0 || line[line_len - 1] != '\n')
	{
		if (buff_offset == 0)
		{
			buff_len = read(fd, buff, BUFFER_SIZE);
			if (buff_len == -1)
			{
				free(line);
				return (NULL);
			}
			if (buff_len == 0)
				break ;
		}
		i = buff_offset;
		while (i < buff_len && buff[i++] != '\n')
			continue ;
		tmp = malloc(line_len + i - buff_offset + 1);
		if (tmp == NULL)
		{
			free(line);
			buff_offset = 0;
			return (NULL);
		}
		i = -1;
		while (++i < line_len)
			tmp[i] = line[i];
		free(line);
		line = tmp;
		while (buff_offset < buff_len && (line_len == 0 || line[line_len - 1] != '\n'))
			line[line_len++] = buff[buff_offset++];
		if (buff_offset == buff_len)
			buff_offset = 0;
		line[line_len] = '\0';
	}
	return (line);
}

#include <fcntl.h>
#include <stdio.h>

static int
int_from_cstr(char *str)
{
	long long	total;

	total = 0;
	while (*str && *str >= '0' && *str <= '9')
	{
		total *=10;
		total += *str - '0';
		str++;
	}
	return (total);
}

int
main(int ac, char **av)
{
	char	*filepath;
	char	*line;
	int		line_number;
	int		fd;
	int		i;	

	if (ac == 1)
	{
		printf("Usage: gnl <filepath> ?<line_number>\n");
		return (1);
	}
	filepath = av[1];
	if (ac == 3)
		line_number = int_from_cstr(av[2]);
	else
		line_number = -1;
	fd = open(filepath, O_RDONLY);
	line = gnl(fd);
	i = 0;
	while (line)
	{
		if (line_number < 0 || i == line_number)
			printf("%s", line);
		free(line);
		if (i++ == line_number)
			break ;
		line = gnl(fd);
	}
	close(fd);
	return (0);
}
