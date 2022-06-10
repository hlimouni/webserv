# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: abdait-m <abdait-m@student.1337.ma>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/04/10 14:58:16 by abdait-m          #+#    #+#              #
#    Updated: 2022/04/21 04:52:58 by abdait-m         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv

HDRS = ./headers/_server_.hpp ./headers/_UltimateHeader_.hpp \
		./headers/configParser.hpp ./headers/HttpRequest.hpp \
		./headers/IndexOfHtmlTemplate.hpp ./headers/HttpResponse.hpp \
		./sources/ReuseSocket.hpp     ./sources/SimpleServer.hpp ./sources/Socket.hpp          ./sources/SocketsPool.hpp     ./sources/WebServer.hpp       ./sources/clientData.hpp      ./sources/listeningSocket.hpp  \
		./headers/location.hpp ./headers/serverData.hpp \

SRCS =  ./sources/main.cpp ./sources/_server_.cpp \
		./sources/configParser.cpp ./sources/HttpRequest.cpp \
		./sources/IndexOfHtmlTemplate.cpp ./sources/HttpResponse.cpp \
		./sources/location.cpp ./sources/serverData.cpp \
		./sources/ReuseSocket.cpp \
		./sources/SimpleServer.cpp \
		./sources/Socket.cpp \
		./sources/SocketsPool.cpp \
		./sources/WebServer.cpp \
		./sources/clientData.cpp \
		./sources/listeningSocket.cpp \



OBJS = $(SRCS:.cpp=.o)


CPP = clang++

CPPFLAGS = -std=c++98 -Wall -Wextra -Werror

all: $(NAME)

$(NAME): $(OBJS)
	@$(CPP) $(OBJS) -o $@
	@echo "\n\033[35mExecutable file has been created .\033[0m\n"

%.o: %.cpp $(HDRS)
	@$(CPP) $(CPPFLAGS) -o $@ -c $<

clean:
	@rm -f $(OBJS)
	@echo "\n\033[31mObjects removed!\033[0m\n"

fclean: clean
	@rm -f $(NAME)

re: fclean all