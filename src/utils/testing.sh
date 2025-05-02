#!/bin/bash

# Define colors
RED="\033[1;31m"
GREEN="\033[1;32m"
BLUE="\033[1;34m"
YELLOW="\033[1;33m"
CYAN="\033[1;36m"
MAGENTA="\033[1;35m"
RESET="\033[0m"

echo -e "${RED}//=====================================//${RESET}"
echo -e "${MAGENTA}//==========INVALID SERVER:==============//${RESET}"
echo -e "${RED}//=====================================//${RESET}"
echo ""
echo ""
echo -e "${RED}//==========BAD_REDIR_TEST:==============//${RESET}"
./webserv bad_redir.conf
echo ""
echo -e "${RED}//==========EMPTY_SERVER_NAME:===========//${RESET}"
./webserv empty_server_name.conf
echo ""
echo -e "${RED}//==========EQAUL_IS_MISSING:===========//${RESET}"
./webserv equal_is_missing.conf
echo ""
echo -e "${RED}//==========MISSING_CURLY:===============//${RESET}"
./webserv missing_curly.conf
echo ""
echo -e "${RED}//==========NEG_MAX_CLIENT_BODY_SIZE:====//${RESET}"
./webserv negative_max_client_body_size.conf
echo ""
echo -e "${RED}//==========NON_VALID_REDIRECT:==========//${RESET}"
./webserv non_existing_conf.conf
echo ""
echo -e "${RED}//==========NON_VALID_SERVER_TIMEOUT:====//${RESET}"
./webserv non_valid_server_timeout.conf
echo ""
echo -e "${RED}//==========NON_VALID_VALUE:=============//${RESET}"
./webserv non_valid_value.conf
echo ""
echo -e "${RED}//==========NOT_ACCEPTED_ATTRIBUTE:======//${RESET}"
./webserv not_accepted_attribute.conf
echo ""
echo -e "${RED}//==========NOT_LOCALHOST:===============//${RESET}"
./webserv not_localhost.conf
echo ""
echo -e "${RED}//==========1000_MAX_CLIENT_BODY_SIZE:===//${RESET}"
./webserv one_thousand_max_client_body_size.conf
echo ""
echo -e "${RED}//==========WRONG_ORDER_CODE_REDIR:======//${RESET}"
./webserv wrong_order_code_redir.conf
echo ""
echo -e "${RED}//==========WRONG_REDIR_CODE:============//${RESET}"
./webserv wrong_redir_code.conf
echo ""
echo -e "${RED}//==========0_MAX_CLIENT_BODY_SIZE:======//${RESET}"
./webserv zero_max_client_body_size.conf
echo -e "${RED}//==========SERVERS_WITH_THE_SAME_PORT:=//${RESET}"
./webserv servers_with_the_same_port.conf
echo ""
echo -e "${GREEN}//==========VALID SERVER:================//${RESET}"
echo -e "${CYAN}//==========SVR_NOT_IN_THE_FIRST_LINE:===//${RESER}"
./webserv not_server_in_the_first_line.conf
echo -e "${GREEN}//==========VALID SERVER:================//${RESET}"
echo -e "${CYAN}//==========RANDOM_SPACES_AND_COMMENTS:===//${RESER}"
./webserv random_spaces_and_comments.conf