#  File:    Makefile
#  Author:  Jeff Moore
#  Date:    Aug 13, 2016
#
#  Makefile for ControlSocket.

DEBUG	= 	-O2
CC	= 	g++-4.7
#CC	= 	gcc
INCLUDE	= 	-I'tinyxml' -I'IvySox'
#FLAGS	= 	$(DEBUG) $(INCLUDE) -g -pthread -std=c++0x -Wall
FLAGS	=	$(DEBUG) $(INCLUDE) -g -pthread -std=c++11 -fpermissive
SOURCE	= 	main.cpp Command.cpp ControlSocket.cpp StringHelperFunctions.cpp IvySox/IvySox.cpp tinyxml/tinyxml2.cpp
OBJ 	=	$(SOURCE:.cpp=.o)
LIBS	=	-lpthread

all:		ControlSocket

ControlSocket:		$(OBJ)
		@echo [Link]
		@$(CC) -o $@ $(OBJ) $(LIBS)

.cpp.o:		
		@echo [Compile] $<
		@$(CC) -c $(FLAGS) $< -o $@

clean:
		@echo "[Clean]"
		@rm -f $(OBJ) ControlSocket

install:
		@echo "[Install]"
