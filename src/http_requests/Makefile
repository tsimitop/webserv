# NAME = a.out

# CC = c++

# CFLAGS = -Wall -Werror -Wextra -std=c++17 -fsanitize=address

# DEBUG = -g

# SRCS = main.cpp HttpRequest.cpp

# OBJ_DIR = objects
# OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.o))

# $(NAME): $(OBJS)
# 	@echo Compiling...
# 	@$(CC) $(DEBUG) $(CFLAGS) $^ -o $@

# $(OBJ_DIR)/%.o: %.cpp
# 	@mkdir -p $(OBJ_DIR)
# 	@$(CC) $(DEBUG) $(CFLAGS) -c $< -o $@

# all: $(NAME)

# clean:
# 	@echo Deleting objects...
# 	@rm -rf $(OBJ_DIR)

# fclean: clean
# 	@echo Deleting executable...
# 	@rm -f $(NAME)

# superclean: fclean
# 	@rm -f *.txt

# re: fclean all

# .PHONY: all clean fclean re