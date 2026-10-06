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

NAME = codexion

CC = cc	
CFLAGS = -Wall -Wextra -Werror -MMD -MP

SRC = parsing.c init.c cleanup.c coder.c dongle.c heap.c\
 	logging.c monitor.c simulation.c time.c utils.c main.c\

OBJ_DIR = obj

OBJ = $(SRC:%.c=$(OBJ_DIR)/%.o)
DEP = $(OBJ:.o=.d)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME) -pthread

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
