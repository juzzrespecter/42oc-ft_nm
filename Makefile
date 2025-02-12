# ~ Project dependencies ~

SRC := nm.c \
       nm_common.c \
       utils.c \
       arg_parser.c \
       elf_parser.c \
       symbol.c \
       x32_parse.c \
       x64_parse.c

INCLUDE := nm.h

SRC_DIR := src/
INCLUDE_DIR := include/ \
               libft/includes/
OBJ_DIR := obj/
OBJ     := ${patsubst %.c, ${OBJ_DIR}%.o, ${SRC}}

LIBFT_DIR := libft/

vpath %.c src/
vpath %.h include/

# ~ Compilation variables ~
CC := gcc
CFLAGS = -Wall -Werror -Wextra
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
	${CC} ${CFLAGS} -c ${SRC_DIR}${notdir $<} -o $@ ${IFLAGS}

${OBJ_DIR}:
	mkdir -v ${OBJ_DIR}

${LIBFT}:
	make -C ${LIBFT_DIR}

clean:
	${RM} ${OBJ_DIR}

fclean: clean
	${RM} ${NAME}

testo:
	echo ${OBJ}

up:
	docker build . -t nm-sandbox
	docker run --name nm-sandbox -it -v .:/nm nm-sandbox bash -c "make test"

down:
	docker stop nm-sandbox
	docker rm nm-sandbox

debug: fclean
	${MAKE} all CFLAGS="${CFLAGS} -g3 -fsanitize=address"

test:
	bash tests.sh

re: fclean all

.PHONY: all clean fclean re test up down debug