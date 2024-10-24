#include <nm.h>

/* allowed functions:
open(2) close(2) mmap(2) munmap(2) write(2)
fstat(2) malloc(3) free(3) exit(3) perror(3)
strerror(1)


*/

int test_id_binaries(char* file)
{
  int fd = open(file, O_RDONLY);

  if (fd < 0)
  {
    perror("TEST");
    return (EXIT_FAILURE);
  }

  return (EXIT_SUCCESS);
}

void check_variables(t_context* ctx)
{
  if (ctx->flags & DEBUG_SYMS_F)
  {
    printf("Debug syms set\n");
  }
  if (ctx->flags & EXTRN_ONLY_F)
  {
    printf("Extern only set\n");
  }
  if (ctx->flags & NO_SORT_F)
  {
    printf("No sort set\n");
  }
  if (ctx->flags & UNDEF_ONLY_F)
  {
    printf("Undefined only set\n");
  }
  if (ctx->flags & REV_SORT_F)
  {
    printf("Reverse sort set\n");
  }
  printf("BINARIOS: \n");
  t_list *test = ctx->bin;
  while (test)
  {
    printf("%s\n", (char*)test->content);
    test = test->next;
  }
}

int main(int argc, char* argv[])
{
  t_context ctx;

  ft_bzero(&ctx, sizeof(ctx));
  arg_parser(&ctx, argc, argv);
  check_variables(&ctx);
  routine(&ctx);
}
