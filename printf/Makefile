# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: cornguye <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/11/17 12:30:33 by cornguye          #+#    #+#              #
#    Updated: 2023/11/17 12:31:01 by cornguye         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = printf.a
CC = cc
CFLAGS = -Wall -Werror -Wextra
SRC = ft_printf.c ft_putstr.c ft_putchar.c ft_putnbr.c
OBJ = $(SRC:.c=.o)

all: $(NAME)

%.o : %.c
	@$(CC) $(CFLAGS) -c $< -o $@
	
$(NAME): $(OBJ)
	@ar rcs $(NAME) $(OBJ)
	
clean :
	@rm -rf *.o

fclean : clean
	@rm -rf $(NAME)
	
re : fclean all
