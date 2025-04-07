NAME = webserv

CC = c++

CFLAGS = -g -Wall -Werror -Wextra -std=c++17 -fsanitize=address

#DEBUG = -g

SOURCE	= ./src

OBJ_DIR	= ./obj

SRCS =	main.cpp \
		sockets/sockets.cpp \
		sockets/Server.cpp \
		http_requests/HttpRequest.cpp \
		http_requests/HttpResponse.cpp \
		config_parser/Http.cpp \
		config_parser/ServerInfo.cpp \
		config_parser/Location.cpp

SRC:=		$(addprefix $(SOURCE)/,$(SRCS))
OBJS:=		$(SRC:$(SOURCE)/%.cpp=$(OBJ_DIR)/%.o)

GREEN  := \033[1;32m
RESET  := \033[0m

all: $(NAME)

$(OBJS): $(OBJ_DIR)%.o : $(SOURCE)%.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling source file $<."
	@$(CC) -c $(CFLAGS) $< -o $@

$(NAME): $(OBJS)
	@echo "Compiling executable."
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)$(NAME) compiled successfully!$(RESET)"

clean:
	@echo Deleting objects.
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo Deleting executable.
	@rm -f $(NAME)

superclean: fclean
	@rm -f *.txt

re: fclean all

.PHONY: all clean fclean re superclean
