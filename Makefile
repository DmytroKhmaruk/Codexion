# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dkhmaruk <dkhmaruk@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/09/18 11:24:41 by dkhmaruk          #+#    #+#              #
#    Updated: 2026/09/18 13:07:18 by dkhmaruk         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = codexion.a

CC = cc	
CFLAGS = -Wall -Wextra -Werror -MMD 

SRC = parsing.c init.c cleanup.c\


OBJ_DIR = obj

OBJ = $(SRC:%.c=$(OBJ_DIR)/%.o)
DEP = $(OBJ:.o=.d)

all: $(NAME)

$(NAME): $(OBJ)
	ar rcs $(NAME) $(OBJ)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
-include $(DEP)
