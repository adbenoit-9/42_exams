# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/05/03 11:21:34 by adbenoit          #+#    #+#              #
#    Updated: 2021/12/03 17:44:29 by adbenoit         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	=	microshell
CC		=	gcc
FLAG	=	-Wall -Wextra -Werror

SRC		=	main.c microshell.c utils.c
OBJ		=	$(SRC:.c=.o)

all:	$(NAME)

$(NAME): $(OBJ)
	@$(CC) $(FLAG) -o $(NAME) $(OBJ)
	@printf "\n"
	@echo "Compilation of \033[33;1m$(NAME)\033[0;1m: [\033[1;32mOK\033[0;1m]\033[0m"

%.o:	%.c
	@printf "\033[34;1m|\033[0;m"
	@mkdir $(OBJ_PATH) 2> /dev/null || true
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm $(OBJ) 2> /dev/null || true
	@echo "\033[33;1m$(NAME)\033[0;1m: objects deleted\033[0m"

fclean: clean
	@rm $(NAME) 2> /dev/null || true
	@echo "\033[33;1m$(NAME)\033[0;1m: $(NAME) deleted\033[0m"

re: fclean all

.PHONY: all clean fclean re
