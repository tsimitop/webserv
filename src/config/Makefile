# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tnakas <tnakas@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/21 16:08:16 by tnakas            #+#    #+#              #
#    Updated: 2025/04/04 17:51:42 by tnakas           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME        = config
SRC_DIR     = .
OBJ_DIR     = obj

SRC         = $(SRC_DIR)/main.cpp $(SRC_DIR)/Http/Http.cpp \
				$(SRC_DIR)/ServerInfo/ServerInfo.cpp $(SRC_DIR)/Location/Location.cpp
OBJ         = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CC          = c++ -g
RM          = rm -rf
STD         = c++17
CPPFLAGS    = -Wall -Wextra -Werror -std=$(STD)

all: $(NAME)

$(NAME): $(OBJ)
	mkdir -p obj
	$(CC) $(CPPFLAGS) $(OBJ) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re