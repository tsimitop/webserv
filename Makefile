NAME = webserv

CC = c++

# CFLAGS = -g -fsanitize=address -std=c++17 -Wall -Werror -Wextra
CFLAGS = -g -std=c++17 -Wall -Werror -Wextra #-fsanitize=address
#DEBUG = -g

SOURCE	= ./src

OBJ_DIR	= ./obj

SRCS =	main.cpp \
		http_requests/HttpRequest.cpp \
		http_requests/HttpResponse.cpp \
		config_parser/Http.cpp \
		config_parser/ServerInfo.cpp \
		config_parser/Location.cpp \
		sockets_and_poll/Poll.cpp \
		sockets_and_poll/PollFdWithFlag.cpp \
		cgis/Cgi.cpp \
		cgis/CgiSingleton.cpp

all: $(NAME) #log_refresh

SRC:=		$(addprefix $(SOURCE)/,$(SRCS))
OBJS:=		$(SRC:$(SOURCE)/%.cpp=$(OBJ_DIR)/%.o)

GREEN  := \033[1;32m
RESET  := \033[0m

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

# log_refresh:
# 	@rm ./webserv.log

re: fclean all

docker_compile:
	docker build -t webserv .

docker_run: docker_compile
	@docker run --cap-add=NET_ADMIN -it -p 4242:4242 -p 4243:4243 -p 4244:4244 -p 4245:4245 --name webserv webserv

docker_it:
	docker exec -it webserv /bin/bash

docker_clean:
	docker stop webserv
	docker rm webserv

docker_re: docker_clean docker_run

.PHONY: all clean fclean re superclean
