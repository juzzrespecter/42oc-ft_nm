#include "nm.h"

/* allowed functions:



*/

int test_id_binaries(char *file)
{
  int fd = open(file, O_RDONLY);

  if (fd < 0)
  {

  }
}


int parse_args(int argc, char *argv[])
{
  char flags = 0;

  const char *valid_arguments[] = {
     ["-a", "--debug-syms"],
     ["-g", "--extern-only"],
     ["-u", "--undefined-only"],
     ["-p", "--no-sort"],
     ["-r", "--reverse-sort"]
  };

}


int main(int argc, char *argv[])
{
    (void) argc;
    (void) argv;


}