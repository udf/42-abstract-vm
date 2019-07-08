# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mhoosen <mhoosen@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2018/01/25 07:39:20 by mhoosen           #+#    #+#              #
#    Updated: 2019/07/03 14:50:47 by mhoosen          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# General settings
NAME=avm

HEADER_EXT=hpp
SRC_EXT=cpp

CXX=clang++
CFLAGS=-Wall -Wextra -Werror -Wconversion --std=c++17
LIB_FLAGS=

# Paths (with trailing /)
INCLUDE_DIR=./include/
SRC_DIR=./srcs/
BUILD_DIR=./build/

# Files that have a matching .cpp and .hpp
FILESTEMS=exceptions OperandFactory

# Plain source files
SRC_FILES=$(FILESTEMS) main

# Plain headers
INCLUDE_FILES=$(FILESTEMS) IOperand OperandFactory Operand OperandOps AVM

# --- script kiddies cut here ---
INCLUDES=$(INCLUDE_FILES:%=$(INCLUDE_DIR)%.$(HEADER_EXT))
SRCS=$(SRC_FILES:%=$(SRC_DIR)%.$(SRC_EXT))
OBJS=$(SRC_FILES:%=$(BUILD_DIR)%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CFLAGS) $(LIB_FLAGS) $(OBJS) -o $@

$(BUILD_DIR)%.o: $(SRC_DIR)%.$(SRC_EXT) $(INCLUDES)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CFLAGS) -I $(INCLUDE_DIR) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

run: all
	./$(NAME)