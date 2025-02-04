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

OBJ_FILE="
static int variable_global_estatica_sin_inicializar;
static int variable_global_estatica_inicializada = 1;
static const int variable_global_estatica_constante = 1;

__attribute__((weak)) int variable_debil;
__attribute__((weak)) int variable_debil_inicializada = 2;

static void function_estatica() {
  static int variable_local_estatica;

  printf(\"Llamada a funcion externa de libreria\");
}

volatile int variable_volatil;
extern int variable_externa;

void function_linkable(int a, int b, char* c) {
  static int variable_local_estatica;

  function_estatica();
}
void declaracion_de_otra_funcion(int a, int b);

__attribute__((weak)) int funcion_debil(int a, int b);
"
SUM_FILE="
int sum(int a, int b) {
  return a + b;
}
"
MAIN_FILE="
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


function exec_test() {
  rm -f logs $FILE_NAME
  export CXX=gcc
  echo -n "Executing test $TEST_NAME ..."
  cat << EOF | $CXX -x c - -c -o $FILE_NAME 2>/dev/null
$ELF_FILE
EOF
  diff <($NM $FLAGS $FILE_NAME 2>&1) <($FT_NM $FLAGS $FILE_NAME 2>&1) >./logs
  if [ "$?" -eq 1 ]; then echo -e "$RED\t[ ko ]$END";cat logs; else echo -e "$GREEN\t[ ok ]$END"; fi
}

function exec_test_cpp() {
  rm -f logs $FILE_NAME
  export CXX=g++
  echo -n "Executing test $TEST_NAME ..."
  cat << EOF | $CXX -x c - -c -o $FILE_NAME 2>/dev/null
$ELF_FILE
EOF
  diff <($NM $FLAGS $FILE_NAME 2>&1) <($FT_NM $FLAGS $FILE_NAME 2>&1) >./logs
  if [ "$?" -eq 1 ]; then echo -e "$RED\t[ ko ]$END";cat logs; else echo -e "$GREEN\t[ ok ]$END"; fi
}

function exec_test_binary_cpp_32() {
  rm -f logs $FILE_NAME
  export CXX=g++
  echo -n "Executing test $TEST_NAME ..."
  cat << EOF | $CXX -m32 -x c - -c -o sum.o 2>/dev/null
$SUM_FILE
EOF
  cat << EOF | $CXX -m32 -x c - -c -o main.o 2>/dev/null
$MAIN_FILE
EOF
  $CXX -m32 sum.o main.o -o $FILE_NAME
  diff <($NM $FLAGS $FILE_NAME 2>&1) <($FT_NM $FLAGS $FILE_NAME 2>&1) >./logs
  if [ "$?" -eq 1 ]; then echo -e "$RED\t[ ko ]$END";cat logs; else echo -e "$GREEN\t[ ok ]$END"; fi
}

# exec test 32

# exec test 32 cpp

# multi w failures

# multi

make

mkdir -vp $TEST_DIR
pushd $TEST_DIR

cp ../$FT_NM .

# tests error no binario

# tests flags incorrectas

# test vacio a.out

echo "[ OBJECT FILE TESTS ]"
ELF_FILE=$OBJ_FILE FLAGS="" TEST_NAME="[ without flags ]" FILE_NAME="test" exec_test
ELF_FILE=$OBJ_FILE FLAGS="-a" TEST_NAME="[ -a flag ]" FILE_NAME="test" exec_test
ELF_FILE=$OBJ_FILE FLAGS="-g" TEST_NAME="[ -g flag ]" FILE_NAME="test" exec_test
ELF_FILE=$OBJ_FILE FLAGS="-u" TEST_NAME="[ -u flag ]" FILE_NAME="test" exec_test
ELF_FILE=$OBJ_FILE FLAGS="-r" TEST_NAME="[ -r flag ]" FILE_NAME="test" exec_test
ELF_FILE=$OBJ_FILE FLAGS="-p" TEST_NAME="[ -p flag ]" FILE_NAME="test" exec_test
#exec_test

echo "[ OBJECT FILE CPP TESTS ]"
ELF_FILE=$OBJ_FILE FLAGS="" TEST_NAME="[ without flags ]" FILE_NAME="test_cpp" exec_test_cpp
ELF_FILE=$OBJ_FILE FLAGS="-a" TEST_NAME="[ -a flag ]" FILE_NAME="test_cpp" exec_test_cpp
ELF_FILE=$OBJ_FILE FLAGS="-g" TEST_NAME="[ -g flag ]" FILE_NAME="test_cpp" exec_test_cpp
ELF_FILE=$OBJ_FILE FLAGS="-u" TEST_NAME="[ -u flag ]" FILE_NAME="test_cpp" exec_test_cpp
ELF_FILE=$OBJ_FILE FLAGS="-r" TEST_NAME="[ -r flag ]" FILE_NAME="test_cpp" exec_test_cpp
ELF_FILE=$OBJ_FILE FLAGS="-p" TEST_NAME="[ -p flag ]" FILE_NAME="test_cpp" exec_test_cpp

popd
rm -frv $TEST_DIR
