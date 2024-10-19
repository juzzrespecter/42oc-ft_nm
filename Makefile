SRC := nm.c
INCLUDE := nm.h

SRC_DIR := src/
INCLUDE_DIR := include/
OBJ_DIR := obj/

vpath %.c src
vpath %.h include

CC := gcc
CFLAGS = -Wall -Werror -Wextra
ifdef DEBUG
    CFLAGS += -fsanitize=address -g3
endif
IFLAGS := -I ${INCLUDE_DIR}

OBJ = ${SRC:%.c=${OBJ_DIR}%.o}

NAME := ft_nm

all: ${NAME}

${NAME}: ${OBJ}
	${CC} ${CFLAGS} $^ -o $@ ${IFLAGS}

${OBJ_DIR}%.o:  ${SRC_DIR}%.c | ${OBJ_DIR}
	${CC} ${CFLAGS} -c $< -o $@ ${IFLAGS}


${OBJ_DIR}:
	mkdir -v ${OBJ_DIR}

clean:
	rm -v ${OBJ}

fclean: clean
	rm -v ${NAME}

re: fclean all
