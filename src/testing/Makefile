NAME = exec

CC = c++

CFLAGS = -Wall -Werror -Wextra -std=c++17 -fsanitize=address

DEBUG = -g

SRCS = main.cpp ../http_requests/HttpRequest.cpp ../http_requests/HttpResponse.cpp HttpRequestTester.cpp

OBJS = $(SRCS:.cpp=.o)

$(NAME) : $(OBJS)
	@echo Compiling...
	@$(CC) $(DEBUG) $(CFLAGS) $^ -o $@

%.o:%.cpp
	@$(CC) $(DEBUG) $(CFLAGS) -c $< -o $@

all : $(NAME)

clean :
	@echo Deleting objects...
	@rm -f $(OBJS)

fclean : clean
	@echo Deleting executable...
	@rm -f $(NAME)

superclean : fclean
	@rm -f *.txt

re: clean all

.PHONY = all clean fclean re