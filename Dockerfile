FROM gcc:latest

RUN apt update && apt install -y valgrind

WORKDIR /usr/src/app

COPY . .

RUN make clean && make
# RUN make

EXPOSE 4242 4243 4244 4245

# ENTRYPOINT ["./webserv"]
ENTRYPOINT ["valgrind", "--leak-check=full", "--show-leak-kinds=all", "./webserv"]
