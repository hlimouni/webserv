NAME = webserv

HDRS = ./headers/configParser.hpp ./headers/HttpRequest.hpp \
		./headers/IndexOfHtmlTemplate.hpp ./headers/HttpResponse.hpp \
		./sources/ReuseSocket.hpp ./sources/Socket.hpp  ./sources/SocketsPool.hpp ./sources/WebServer.hpp       ./sources/clientData.hpp      ./sources/listeningSocket.hpp  \
		./headers/location.hpp ./headers/serverData.hpp \

SRCS =  ./sources/main.cpp \
		./sources/configParser.cpp ./sources/HttpRequest.cpp \
		./sources/IndexOfHtmlTemplate.cpp ./sources/HttpResponse.cpp \
		./sources/location.cpp ./sources/serverData.cpp \
		./sources/ReuseSocket.cpp \
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
	@echo "\nExecutable file has been created .\n"

%.o: %.cpp $(HDRS)
	@$(CPP) $(CPPFLAGS) -o $@ -c $<

clean:
	@rm -f $(OBJS)
	@echo "\nObjects removed!\n"

fclean: clean
	@rm -f $(NAME)

re: fclean all