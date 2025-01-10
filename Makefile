# ~ Project dependencies ~

SRC := nm.c \
       utils.c \
       arg_parser.c \
       elf_parser.c \
       parse_x32/header_x32.c \
       parse_x32/section_x32.c \
       parse_x64/header_x64.c \
       parse_x64/section_x64.c

INCLUDE := nm.h

SRC_DIR := src/
INCLUDE_DIR := include/ \
               libft/includes/
OBJ_DIR := obj/
OBJ     := ${patsubst ${SRC_DIR}%.c, ${OBJ_DIR}%.o, ${SRC}}

LIBFT_DIR := libft/

vpath %.c src/
vpath %.h include/

# ~ Compilation variables ~
CC := gcc
CFLAGS = -Wall -Werror -Wextra
ifdef DEBUG
    CFLAGS += -fsanitize=address -g3
endif
IFLAGS := $(addprefix -I, ${INCLUDE_DIR})
LFLAGS := -lft -L${LIBFT_DIR}

LIBFT     := ${addprefix ${LIBFT_DIR}, libft.a}

RM := rm -rfv

NAME := ft_nm

# ~ Rules ~

all: ${NAME}

${NAME}: ${OBJ} ${LIBFT}
	${CC} ${CFLAGS} $^ -o $@ ${IFLAGS} ${LFLAGS}

${OBJ_DIR}%.o:  ${SRC_DIR}%.c | ${OBJ_DIR}
	${CC} ${CFLAGS} -c $< -o $@ ${IFLAGS}

${OBJ_DIR}:
	mkdir -v ${OBJ_DIR}

${LIBFT}:
	make -C ${LIBFT_DIR}

clean:
	${RM} ${OBJ}

fclean: clean
	${RM} ${NAME}

test:
	echo ${OBJ}

re: fclean all

.PHONY: all clean fclean re