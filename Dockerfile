FROM gcc:latest

RUN apt update && apt install -y valgrind telnet curl siege

WORKDIR /usr/src/app

COPY . .

RUN make clean && make

EXPOSE 4242 4243 4244 4245

CMD ["valgrind", "--leak-check=full", "--show-leak-kinds=all", "./webserv"]
