#!/bin/bash

export LC_COLLATE='C'
NM=nm
FT_NM=ft_nm

TEST_DIR=tests
CXX=gcc
GREEN="\033[32m"
RED="\033[31m"
END="\033[0m"

OBJ_FILE="<< EOF
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
EOF"

SUM_FILE="<< EOF
int sum(int a, int b) {
  return a + b;
}
EOF"
MAIN_FILE="<< EOF
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
EOF"


function exec_test() {
  echo "Executing test $TEST_NAME..."
  #cat $ELF_FILE | $CXX -x c -c $FILE_NAME
  #diff <($NM $FLAGS) <($FT_NM $FLAGS) >>./logs
  #if [ "$?" -eq "1" ]; then
  #  echo RED "\t[ ko ]" END
  #else
  #  echo GREEN "\t[ ok ]" END
}


# creamos carpeta de binaros
mkdir -vp $TEST_DIR
pushd $TEST_DIR

echo "[ OBJECT FILES TESTS ]"
ELF_FILE=$OBJ_FILE
FLAGS=""
TEST_NAME="[ without flags ]"
exec_test
FLAGS="-a"
TEST_NAME="[ -a flag ]"
exec_test

popd
rm -fv $TEST_DIR
# tests error no binario

# tests error flags


# generamos el objeto
# compilamos el objeto
# lanzamos test
# retreievamos valor de retorno
# imprimimos error y "error"