NAME = libgnl.a
CC = cc
CFLAGS = -Wall -Wextra -Werror
CPPFLAGS =
SRC = get_next_line.c get_next_line_utils.c
OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	ar rcs $@ $^

%.o: %.c get_next_line.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

example: $(NAME)
	$(CC) $(CPPFLAGS) $(CFLAGS) -I. examples/read_lines.c $(NAME) -o gnl_example

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME) gnl_example

re: fclean all

.PHONY: all example clean fclean re
