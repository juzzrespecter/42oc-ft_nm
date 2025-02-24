#!/bin/bash

export LC_COLLATE='C'
NM=nm
FT_NM=./ft_nm

TEST_DIR=tests
CXX=gcc
GREEN="\033[32m"
RED="\033[31m"
END="\033[0m"

EMPTY_FILE="
"

SUM_FILE="
int sum(int a, int b) {
  return a + b;
}
"
MAIN_CPP_FILE="
# include <stdexcept>
# include <iostream>
int sum(int a, int b);

int main() {
  try {
     static int a = 3;
     static int b = 4;

     std::cout << sum(a, b) << std::endl;
  } catch(std::exception& e) {
     std::cerr << e.what() << std::endl;
     return 1;
  }
  return 0;
}
"
MAIN_C_FILE="
# include <stdio.h>
int sum(int a, int b);

int main() {
  static int a;

  a = sum(1, 2);
  printf(\"Result: %d\n\", a);
  return 0;
}
"

OBJ_C_FILE="
# include <stdio.h>
extern int simbolo_marcado_como_externo;

asm(\".globl variable_absoluta\");
asm(\"variable_absoluta = 0x42\");
__attribute__((weak)) void funcion_debil();
__attribute__((weak)) int variable_debil;
int variable_global;
int variable_global_inicializada = 4;
const int variable_global_constante = 3;
static char *variable_estatica_global = \"test command\";
void funcion_sin_definir();
static const char* variable_estatica_global_constante = \"hola\";
void funcion_definida_linkable() {
  extern const int variable_local_constante;
  static const int variable_estatica_constante_local = '3';
  static char *variable_estatica_local = \"test command\";
  return ;
}
volatile int variable_volatil;
__attribute__((weak))
int funcion_debil_definida() {
	printf(\"Dale\");
}
void *funcion_indirecta_resuelta(void)
{
        printf(\"Void resolvido\n\");
}
void funcion_indirecta(unsigned long bar, ...) __attribute__((ifunc(\"funcion_indirecta_resuelta\")));
"

OBJ_CPP_FILE="
# include <stdio.h>
extern int simbolo_marcado_como_externo;

asm(\".globl variable_absoluta\");
asm(\"variable_absoluta = 0x42\");
__attribute__((weak)) void funcion_debil();
__attribute__((weak)) int variable_debil;
int variable_global;
int variable_global_inicializada = 4;
const int variable_global_constante = 3;
static char *variable_estatica_global = \"test command\";
void funcion_sin_definir();
static const char* variable_estatica_global_constante = \"hola\";
void funcion_definida_linkable() {
  extern const int variable_local_constante;
  static const int variable_estatica_constante_local = '3';
  static char *variable_estatica_local = \"test command\";
  return ;
}
volatile int variable_volatil;
__attribute__((weak))
int funcion_debil_definida() {
	printf(\"Dale\");
}
"

function exec_test_obj_file() {
  rm -f logs $FILE_NAME
  echo -n "Executing test $TEST_NAME ..."
  cat << EOF | $CXX $ARCH_FLAG -x $TYPE - -c -o $FILE_NAME 2>/dev/null
$OBJ_FILE
EOF
  diff <($NM $FLAGS $FILE_NAME 2>&1) <($FT_NM $FLAGS $FILE_NAME 2>&1) >./logs
  if [ "$?" -eq 1 ]; then echo -e "$RED\t[ ko ]$END";cat logs; else echo -e "$GREEN\t[ ok ]$END"; fi
}

function exec_test_binary() {
  rm -f logs $FILE_NAME
  echo -n "Executing test $TEST_NAME ..."
  cat << EOF | $CXX $ARCH_FLAG -x $TYPE - -c -o sum.o 2>/dev/null
$SUM_FILE
EOF
  cat << EOF | $CXX $ARCH_FLAG -x $TYPE - -c -o main.o 2>/dev/null
$MAIN_FILE
EOF
  $CXX $ARCH_FLAG sum.o main.o -o $FILE_NAME
  diff <($NM $FLAGS $FILE_NAME 2>&1) <($FT_NM $FLAGS $FILE_NAME 2>&1) >./logs
  if [ "$?" -eq 1 ]; then echo -e "$RED\t[ ko ]$END";cat logs; else echo -e "$GREEN\t[ ok ]$END"; fi
}

function exec_test_so() {
  rm -f logs $FILE_NAME
  echo -n "Executing test $TEST_NAME ..."
  cat << EOF | $CXX $ARCH_FLAG -shared -fPIC -x $TYPE - -o $FILE_NAME 2>/dev/null
$OBJ_FILE
EOF
  diff <($NM $FLAGS $FILE_NAME 2>&1) <($FT_NM $FLAGS $FILE_NAME 2>&1) >./logs
  if [ "$?" -eq 1 ]; then echo -e "$RED\t[ ko ]$END";cat logs; else echo -e "$GREEN\t[ ok ]$END"; fi
}

function exec_multiple() {
  rm -f logs exec_file_m32 obj_file.o
  echo "int main() { static int variable = 0; printf(\"Lib func call\n\"); }" | gcc -m32 -x c - -o exec_file_m32 2>/dev/null
  echo "const char *const_global_var=\"Const global var\";static int static_global_var; void undefined_func();" | g++ -x c++ - -c -o obj_file.o 2>/dev/null
  ARGS="-a -r exec_file_m32 obj_file.o exec_file_m32"
  diff <($NM $ARGS 2>&1) <($FT_NM $ARGS 2>&1) >./logs
  if [ "$?" -eq 1 ]; then echo -e "$RED\t[ ko ]$END";cat logs; else echo -e "$GREEN\t[ ok ]$END"; fi
}

function exec_multiple_w_fails() {
  rm -f logs exec_file_m32 obj_file.o
  echo "int main() { static int variable = 0; printf(\"Lib func call\n\"); }" | gcc -m32 -x c - -o exec_file_m32 2>/dev/null
  echo "const char *const_global_var=\"Const global var\";static int static_global_var; void undefined_func();" | g++ -x c++ - -c -o obj_file.o 2>/dev/null
  echo "text file" > text_file
  ARGS="-a -r exec_file_m32 bad_file obj_file.o exec_file_m32 text_file"
  diff <($NM $ARGS 2>&1) <($FT_NM $ARGS 2>&1) >./logs
  if [ "$?" -eq 1 ]; then echo -e "$RED\t[ ko ]$END";cat logs; else echo -e "$GREEN\t[ ok ]$END"; fi
}

function exec_test_empty_file_m32() {
  export TEST_NAME FLAGS CXX=gcc OBJ_FILE=$EMPTY_FILE ARCH_FLAG=-m32 FILE_NAME=test_file.o TYPE='c'
  exec_test_obj_file
}

function exec_test_empty_file_m64() {
  export TEST_NAME FLAGS CXX=gcc OBJ_FILE=$EMPTY_FILE ARCH_FLAG=-m32 FILE_NAME=test_file.o TYPE='c'
  exec_test_obj_file
}

function exec_test_obj_file_c_m32() {
  export TEST_NAME FLAGS CXX=gcc OBJ_FILE=$OBJ_C_FILE ARCH_FLAG=-m32 FILE_NAME=test_file.o TYPE='c'
  exec_test_obj_file
}

function exec_test_obj_file_c_m64() {
  export TEST_NAME FLAGS CXX=gcc OBJ_FILE=$OBJ_C_FILE  ARCH_FLAG=-m64 FILE_NAME=test_file.o TYPE='c'
  exec_test_obj_file
}

function exec_test_obj_file_cpp_m32() {
  export TEST_NAME FLAGS CXX=g++ OBJ_FILE=$OBJ_CPP_FILE  ARCH_FLAG=-m32 FILE_NAME=test_file.o TYPE='c++'
  exec_test_obj_file
}

function exec_test_obj_file_cpp_m64() {
  export TEST_NAME FLAGS CXX=g++ OBJ_FILE=$OBJ_CPP_FILE ARCH_FLAG=-m64 FILE_NAME=test_file.o TYPE='c++'
  exec_test_obj_file
}

function exec_test_binary_c_m32() {
  export TEST_NAME FLAGS CXX=gcc MAIN_FILE=$MAIN_C_FILE ARCH_FLAG=-m32 FILE_NAME=test_file TYPE='c'
  exec_test_binary
}

function exec_test_binary_c_m64() {
  export TEST_NAME FLAGS CXX=gcc MAIN_FILE=$MAIN_C_FILE ARCH_FLAG=-m64 FILE_NAME=test_file TYPE='c'
  exec_test_binary
}

function exec_test_binary_cpp_m32() {
  export TEST_NAME FLAGS CXX=g++ MAIN_FILE=$MAIN_CPP_FILE ARCH_FLAG=-m32 FILE_NAME=test_file TYPE='c++'
  exec_test_binary
}

function exec_test_binary_cpp_m64() {
  export TEST_NAME FLAGS CXX=g++ MAIN_FILE=$MAIN_CPP_FILE ARCH_FLAG=-m64 FILE_NAME=test_file TYPE='c++'
  exec_test_binary
}

function exec_test_so_c_m32() {
  export TEST_NAME FLAGS CXX=gcc OBJ_FILE=$OBJ_C_FILE ARCH_FLAG=-m32 FILE_NAME=test_file TYPE='c'
  exec_test_so
}

function exec_test_so_c_m64() {
  export TEST_NAME FLAGS CXX=gcc OBJ_FILE=$OBJ_C_FILE ARCH_FLAG=-m64 FILE_NAME=test_file TYPE='c'
  exec_test_so
}

function exec_test_so_cpp_m32() {
  export TEST_NAME FLAGS CXX=g++ OBJ_FILE=$OBJ_CPP_FILE ARCH_FLAG=-m32 FILE_NAME=test_file TYPE='c++'
  exec_test_so
}

function exec_test_so_cpp_m64() {
  export TEST_NAME FLAGS CXX=g++ OBJ_FILE=$OBJ_CPP_FILE ARCH_FLAG=-m64 FILE_NAME=test_file TYPE='c++'
  exec_test_so
}

function exec_test_no_symbols() {
  diff <($NM $(which $NM) 2>&1) <($FT_NM $(which $NM) 2>&1) >./logs
  if [ "$?" -eq 1 ]; then echo -e "$RED\t[ ko ]$END";cat logs; else echo -e "$GREEN\t[ ok ]$END"; fi
}

make re

mkdir -vp $TEST_DIR
pushd $TEST_DIR

cp ../$FT_NM .

echo "[ TEST NO ARGS ]"
$FT_NM

echo "[ TEST FLAGS NO ARGS ]"
$FT_NM -a -g -u

echo "[ TEST FORMAT NOT RECOGNIZED ]"
cat /dev/random | head -50 > file_bad_format
$FT_NM file_bad_format

echo "[ TEST BAD FLAGS ]"
$FT_NM -h -o -l -a

echo "[ TEST EMPTY BINARY ]"
TEST_NAME="[ no flag ]" FLAGS="" exec_test_empty_file_m32
TEST_NAME="[ no flag ]" FLAGS="" exec_test_empty_file_m64
TEST_NAME="[ -a flag ]" FLAGS="-a" exec_test_empty_file_m32
TEST_NAME="[ -a flag ]" FLAGS="-a" exec_test_empty_file_m64

echo;echo;echo "[ OBJECT FILE TESTS 32 ]"
TEST_NAME="[ no flag ]" FLAGS="" exec_test_obj_file_c_m32
TEST_NAME="[ -a flag ]" FLAGS="-a" exec_test_obj_file_c_m32
TEST_NAME="[ -g flag ]" FLAGS="-g" exec_test_obj_file_c_m32
TEST_NAME="[ -u flag ]" FLAGS="-u" exec_test_obj_file_c_m32
TEST_NAME="[ -r flag ]" FLAGS="-r" exec_test_obj_file_c_m32
TEST_NAME="[ -p flag ]" FLAGS="-p" exec_test_obj_file_c_m32
echo
TEST_NAME="[ -a -r -u flag ]" FLAGS="-a -r -u" exec_test_obj_file_c_m32
TEST_NAME="[ -a -r -p flag ]" FLAGS="-a -r -p" exec_test_obj_file_c_m32
TEST_NAME="[ -u -p -g flag ]" FLAGS="-u -p -g" exec_test_obj_file_c_m32
TEST_NAME="[ -a -u flag    ]" FLAGS="-a -u" exec_test_obj_file_c_m32

echo;echo;echo "[ OBJECT FILE TESTS 64 ]"
FLAGS="" TEST_NAME="[ no flag ]" exec_test_obj_file_c_m64
FLAGS="-a" TEST_NAME="[ -a flag ]" exec_test_obj_file_c_m64
FLAGS="-g" TEST_NAME="[ -g flag ]" exec_test_obj_file_c_m64
FLAGS="-u" TEST_NAME="[ -u flag ]" exec_test_obj_file_c_m64
FLAGS="-r" TEST_NAME="[ -r flag ]" exec_test_obj_file_c_m64
FLAGS="-p" TEST_NAME="[ -p flag ]" exec_test_obj_file_c_m64
echo
TEST_NAME="[ -a -r -u flag ]" FLAGS="-a -r -u" exec_test_obj_file_c_m64
TEST_NAME="[ -a -r -p flag ]" FLAGS="-a -r -p" exec_test_obj_file_c_m64
TEST_NAME="[ -u -p -g flag ]" FLAGS="-u -p -g" exec_test_obj_file_c_m64
TEST_NAME="[ -a -u flag    ]" FLAGS="-a -u" exec_test_obj_file_c_m64

echo;echo;echo "[ OBJECT FILE CPP TESTS 32 ]"
TEST_NAME="[ no flag ]" FLAGS="" exec_test_obj_file_cpp_m32
TEST_NAME="[ -a flag ]" FLAGS="-a" exec_test_obj_file_cpp_m32
TEST_NAME="[ -g flag ]" FLAGS="-g" exec_test_obj_file_cpp_m32
TEST_NAME="[ -u flag ]" FLAGS="-u" exec_test_obj_file_cpp_m32
TEST_NAME="[ -r flag ]" FLAGS="-r" exec_test_obj_file_cpp_m32
TEST_NAME="[ -p flag ]" FLAGS="-p" exec_test_obj_file_cpp_m32
echo
TEST_NAME="[ -a -r -u flag ]" FLAGS="-a -r -u" exec_test_obj_file_cpp_m32
TEST_NAME="[ -a -r -p flag ]" FLAGS="-a -r -p" exec_test_obj_file_cpp_m32
TEST_NAME="[ -u -p -g flag ]" FLAGS="-u -p -g" exec_test_obj_file_cpp_m32
TEST_NAME="[ -a -u flag    ]" FLAGS="-a -u" exec_test_obj_file_cpp_m32

echo;echo;echo "[ OBJECT FILE CPP TESTS 64 ]"
FLAGS="" TEST_NAME="[ no flag ]" exec_test_obj_file_cpp_m64
FLAGS="-a" TEST_NAME="[ -a flag ]" exec_test_obj_file_cpp_m64
FLAGS="-g" TEST_NAME="[ -g flag ]" exec_test_obj_file_cpp_m64
FLAGS="-u" TEST_NAME="[ -u flag ]" exec_test_obj_file_cpp_m64
FLAGS="-r" TEST_NAME="[ -r flag ]" exec_test_obj_file_cpp_m64
FLAGS="-p" TEST_NAME="[ -p flag ]" exec_test_obj_file_cpp_m64
echo
TEST_NAME="[ -a -r -u flag ]" FLAGS="-a -r -u" exec_test_obj_file_cpp_m64
TEST_NAME="[ -a -r -p flag ]" FLAGS="-a -r -p" exec_test_obj_file_cpp_m64
TEST_NAME="[ -u -p -g flag ]" FLAGS="-u -p -g" exec_test_obj_file_cpp_m64
TEST_NAME="[ -a -u flag    ]" FLAGS="-a -u" exec_test_obj_file_cpp_m64

echo;echo;echo "[ EXEC FILE TESTS 32 ]"
TEST_NAME="[ no flag ]" FLAGS="" exec_test_binary_c_m32
TEST_NAME="[ -a flag ]" FLAGS="-a" exec_test_binary_c_m32
TEST_NAME="[ -g flag ]" FLAGS="-g" exec_test_binary_c_m32
TEST_NAME="[ -u flag ]" FLAGS="-u" exec_test_binary_c_m32
TEST_NAME="[ -r flag ]" FLAGS="-r" exec_test_binary_c_m32
TEST_NAME="[ -p flag ]" FLAGS="-p" exec_test_binary_c_m32
echo
TEST_NAME="[ -a -r -u flag ]" FLAGS="-a -r -u" exec_test_binary_c_m32
TEST_NAME="[ -a -r -p flag ]" FLAGS="-a -r -p" exec_test_binary_c_m32
TEST_NAME="[ -u -p -g flag ]" FLAGS="-u -p -g" exec_test_binary_c_m32
TEST_NAME="[ -a -u flag    ]" FLAGS="-a -u" exec_test_binary_c_m32

echo;echo;echo "[ EXEC FILE TESTS 64 ]"
TEST_NAME="[ no flag ]" FLAGS="" exec_test_binary_c_m64
TEST_NAME="[ -a flag ]" FLAGS="-a" exec_test_binary_c_m64
TEST_NAME="[ -g flag ]" FLAGS="-g" exec_test_binary_c_m64
TEST_NAME="[ -u flag ]" FLAGS="-u" exec_test_binary_c_m64
TEST_NAME="[ -r flag ]" FLAGS="-r" exec_test_binary_c_m64
TEST_NAME="[ -p flag ]" FLAGS="-p" exec_test_binary_c_m64
echo
TEST_NAME="[ -a -r -u flag ]" FLAGS="-a -r -u" exec_test_binary_c_m64
TEST_NAME="[ -a -r -p flag ]" FLAGS="-a -r -p" exec_test_binary_c_m64
TEST_NAME="[ -u -p -g flag ]" FLAGS="-u -p -g" exec_test_binary_c_m64
TEST_NAME="[ -a -u flag    ]" FLAGS="-a -u" exec_test_binary_c_m64

echo;echo;echo "[ EXEC FILE CPP TESTS 32 ]"
TEST_NAME="[ no flag ]" FLAGS="" exec_test_binary_cpp_m32
TEST_NAME="[ -a flag ]" FLAGS="-a" exec_test_binary_cpp_m32
TEST_NAME="[ -g flag ]" FLAGS="-g" exec_test_binary_cpp_m32
TEST_NAME="[ -u flag ]" FLAGS="-u" exec_test_binary_cpp_m32
TEST_NAME="[ -r flag ]" FLAGS="-r" exec_test_binary_cpp_m32
TEST_NAME="[ -p flag ]" FLAGS="-p" exec_test_binary_cpp_m32
echo
TEST_NAME="[ -a -r -u flag ]" FLAGS="-a -r -u" exec_test_binary_cpp_m32
TEST_NAME="[ -a -r -p flag ]" FLAGS="-a -r -p" exec_test_binary_cpp_m32
TEST_NAME="[ -u -p -g flag ]" FLAGS="-u -p -g" exec_test_binary_cpp_m32
TEST_NAME="[ -a -u flag    ]" FLAGS="-a -u" exec_test_binary_cpp_m32

echo;echo;echo "[ EXEC FILE CPP TESTS 64 ]"
TEST_NAME="[ no flag ]" FLAGS="" exec_test_binary_cpp_m64
TEST_NAME="[ -a flag ]" FLAGS="-a" exec_test_binary_cpp_m64
TEST_NAME="[ -g flag ]" FLAGS="-g" exec_test_binary_cpp_m64
TEST_NAME="[ -u flag ]" FLAGS="-u" exec_test_binary_cpp_m64
TEST_NAME="[ -r flag ]" FLAGS="-r" exec_test_binary_cpp_m64
TEST_NAME="[ -p flag ]" FLAGS="-p" exec_test_binary_cpp_m64
echo
TEST_NAME="[ -a -r -u flag ]" FLAGS="-a -r -u" exec_test_binary_cpp_m64
TEST_NAME="[ -a -r -p flag ]" FLAGS="-a -r -p" exec_test_binary_cpp_m64
TEST_NAME="[ -u -p -g flag ]" FLAGS="-u -p -g" exec_test_binary_cpp_m64
TEST_NAME="[ -a -u flag    ]" FLAGS="-a -u" exec_test_binary_cpp_m64

echo;echo;echo "[ SO FILE TESTS 32 ]"
TEST_NAME="[ no flag ]" FLAGS="" exec_test_so_c_m32
TEST_NAME="[ -a flag ]" FLAGS="-a" exec_test_so_c_m32
TEST_NAME="[ -g flag ]" FLAGS="-g" exec_test_so_c_m32
TEST_NAME="[ -u flag ]" FLAGS="-u" exec_test_so_c_m32
TEST_NAME="[ -r flag ]" FLAGS="-r" exec_test_so_c_m32
TEST_NAME="[ -p flag ]" FLAGS="-p" exec_test_so_c_m32
echo
TEST_NAME="[ -a -r -u flag ]" FLAGS="-a -r -u" exec_test_binary_cpp_m64
TEST_NAME="[ -a -r -p flag ]" FLAGS="-a -r -p" exec_test_binary_cpp_m64
TEST_NAME="[ -u -p -g flag ]" FLAGS="-u -p -g" exec_test_binary_cpp_m64
TEST_NAME="[ -a -u flag    ]" FLAGS="-a -u" exec_test_binary_cpp_m64

echo;echo;echo "[ SO FILE TESTS 64 ]"
TEST_NAME="[ no flag ]" FLAGS="" exec_test_so_c_m64
TEST_NAME="[ -a flag ]" FLAGS="-a" exec_test_so_c_m64
TEST_NAME="[ -g flag ]" FLAGS="-g" exec_test_so_c_m64
TEST_NAME="[ -u flag ]" FLAGS="-u" exec_test_so_c_m64
TEST_NAME="[ -r flag ]" FLAGS="-r" exec_test_so_c_m64
TEST_NAME="[ -p flag ]" FLAGS="-p" exec_test_so_c_m64
echo
TEST_NAME="[ -a -r -u flag ]" FLAGS="-a -r -u" exec_test_so_c_m64
TEST_NAME="[ -a -r -p flag ]" FLAGS="-a -r -p" exec_test_so_c_m64
TEST_NAME="[ -u -p -g flag ]" FLAGS="-u -p -g" exec_test_so_c_m64
TEST_NAME="[ -a -u flag    ]" FLAGS="-a -u" exec_test_so_c_m64

echo;echo;echo "[ SO FILE CPP TESTS 32 ]"
TEST_NAME="[ no flag ]" FLAGS="" exec_test_so_cpp_m32
TEST_NAME="[ -a flag ]" FLAGS="-a" exec_test_so_cpp_m32
TEST_NAME="[ -g flag ]" FLAGS="-g" exec_test_so_cpp_m32
TEST_NAME="[ -u flag ]" FLAGS="-u" exec_test_so_cpp_m32
TEST_NAME="[ -r flag ]" FLAGS="-r" exec_test_so_cpp_m32
TEST_NAME="[ -p flag ]" FLAGS="-p" exec_test_so_cpp_m32
echo
TEST_NAME="[ -a -r -u flag ]" FLAGS="-a -r -u" exec_test_so_cpp_m32
TEST_NAME="[ -a -r -p flag ]" FLAGS="-a -r -p" exec_test_so_cpp_m32
TEST_NAME="[ -u -p -g flag ]" FLAGS="-u -p -g" exec_test_so_cpp_m32
TEST_NAME="[ -a -u flag    ]" FLAGS="-a -u" exec_test_so_cpp_m32

echo;echo;echo "[ SO FILE CPP TESTS 64 ]"
TEST_NAME="[ no flag ]" FLAGS="" exec_test_so_cpp_m64
TEST_NAME="[ -a flag ]" FLAGS="-a" exec_test_so_cpp_m64
TEST_NAME="[ -g flag ]" FLAGS="-g" exec_test_so_cpp_m64
TEST_NAME="[ -u flag ]" FLAGS="-u" exec_test_so_cpp_m64
TEST_NAME="[ -r flag ]" FLAGS="-r" exec_test_so_cpp_m64
TEST_NAME="[ -p flag ]" FLAGS="-p" exec_test_so_cpp_m64
echo
TEST_NAME="[ -a -r -u flag ]" FLAGS="-a -r -u" exec_test_so_cpp_m64
TEST_NAME="[ -a -r -p flag ]" FLAGS="-a -r -p" exec_test_so_cpp_m64
TEST_NAME="[ -u -p -g flag ]" FLAGS="-u -p -g" exec_test_so_cpp_m64
TEST_NAME="[ -a -u flag    ]" FLAGS="-a -u" exec_test_so_cpp_m64

echo;echo;echo -n -e "[ MULTIPLE FILES ]           ... "
exec_multiple

echo;echo;echo -n -e "[ MULTIPLE FILES W INVALID ] ... "
exec_multiple_w_fails

echo;echo;echo -n -e "[ NO SYMBOLS ]               ... "
exec_test_no_symbols

popd
rm -frv $TEST_DIR
