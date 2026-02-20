# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tvithara <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/10/11 17:13:16 by tvithara          #+#    #+#              #
#    Updated: 2025/10/11 17:13:29 by tvithara         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Colors for output

GREEN	= \033[0;32m
RED		= \033[0;31m
YELLOW	= \033[0;33m
CYAN	= \033[0;36m
NC		= \033[0m


NAME	= philo

CC		= cc
CFLAGS	= -Wall -Wextra -Werror -pthread  # -pthread is safe to leave in CFLAGS for both compilation and linking ???

OBJ_DIR	= obj

SRC		= main.c \
		  init.c \
		  routine.c \
		  monitor.c \
		  actions.c \
		  utils.c

OBJ		= $(SRC:%.c=$(OBJ_DIR)/%.o)

# Rules
all: $(NAME)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "$(CYAN)Compiled:$(NC) $<"

$(NAME): $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME)
	@echo "$(GREEN)✓ $(NAME) compiled successfully!$(NC)"

clean:
	@rm -rf $(OBJ_DIR)
	@echo "$(YELLOW)Object files removed.$(NC)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(RED)$(NAME) removed.$(NC)"

re: fclean all

.PHONY: all clean fclean re