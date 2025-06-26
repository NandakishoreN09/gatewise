
# This file is a part of the wiringBone library
# Updated Makefile for PWM and PRU integration

# Makefile for wiringBone project

# Updated Makefile for resolving Print.h issue
OBJ_DIR = ../BUILD/
SRC_SEARCH_DIR = ../
INCLUDE_DIR = ./library

CPPFLAGS = -Wall -g -I$(INCLUDE_DIR) -I../ -lpthread
CFLAGS = -Wall -g -I$(INCLUDE_DIR) -I../ -lpthread

# Sources
CPP_SRC += $(wildcard *.cpp) $(wildcard library/*.cpp)

MAIN_OBJ = $(addprefix $(OBJ_DIR), $(notdir $(CPP_SRC:.cpp=.o)))

# Rules
all: start main

start:
	@mkdir -p $(OBJ_DIR)

main: $(MAIN_OBJ)
	@echo Compiling Main executable
	@g++ $(MAIN_OBJ) $(CPPFLAGS) -o $(OBJ_DIR)main

$(OBJ_DIR)%.o : %.cpp
	@echo Compiling $(notdir $<)
	@g++ -c $< $(CPPFLAGS) -o $@

$(OBJ_DIR)%.o : library/%.cpp
	@echo Compiling $(notdir $<)
	@g++ -c $< $(CPPFLAGS) -o $@

clean:
	rm -rf $(OBJ_DIR)*
