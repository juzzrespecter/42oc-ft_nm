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
  printf("BINARIOS: ");
  t_list *test = ctx->bin;
  while (test)
  {
    printf(test->content);
    test = test->next;
  }
}

void print_usage_and_exit()
{
  static const char* usage =
    "usage: nm [-a|--debug-syms] [-g|--extern-only]";

  write(STDIN_FILENO, usage, ft_strlen(usage));
  exit(EXIT_FAILURE);
}

int parse_args(t_context* ctx, int argc, char* argv[])
{
  const char* valid_arguments[5][2] = {
    {"-a", "--debug-syms"},
    {"-g", "--extern-only"},
    {"-u", "--undefined-only"},
    {"-p", "--no-sort"},
    {"-r", "--reverse-sort"}
  };
  const char set[5] = {
    DEBUG_SYMS_F,
    EXTRN_ONLY_F,
    UNDEF_ONLY_F,
    NO_SORT_F,
    REV_SORT_F
  };

  if (argc < 2)
  {
    // default mode becomes a.out no flags
  }

  //int i = 1;
  while (*argv)
  {
    // if hyphen, flag
    // else, binary to list
    if (*argv[0] == '-')
    {
      for (int i = 0; i < 5; i++)
      {
        const char* f_short = valid_arguments[i][0];
        const char* f_long = valid_arguments[i][1];
        const size_t len = ft_strlen(*argv);

        if (ft_strncmp(*argv, f_short, len) ||
          ft_strncmp(*argv, f_long, len))
        {
          ctx->flags = ctx->flags | set[i];
          break ;
        }
      }
      print_usage_and_exit();
    }
    else
    {
      ft_lstadd_back(&ctx->bin, ft_lstnew(*argv));
    }
    // print usage with invalid hyphen lead arg
  }
  return 2;
}

int main(int argc, char* argv[])
{
  t_context ctx;

  ft_bzero(&ctx, sizeof(ctx));
  parse_args(&ctx, argc, ++argv);
}
