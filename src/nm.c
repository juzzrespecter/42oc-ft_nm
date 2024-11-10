#include <nm.h>

/* allowed functions:
open(2) close(2) mmap(2) munmap(2) write(2)
fstat(2) malloc(3) free(3) exit(3) perror(3)
strerror(1)


*/

static retval __file_validate_type(t_stat *stat)
{
  if ((stat->st_mode & S_IFMT) == S_IFDIR)
  {
    return ERR_DIR;
  }
  if ((stat->st_mode & S_IFMT) != S_IFREG)
    return ERR_NOFMT;
  return NO_ERR;
}

int main(int argc, char* argv[])
{
  t_context ctx;
  t_list*   bin;

  ft_bzero(&ctx, sizeof(ctx));
  arg_parser(&ctx, argc, argv);

  bin = ctx.bin;
  while(bin)
  {
    char*  bin_path = (char *)bin->content;
    int    bin_fd = open(bin_path, O_RDONLY);
    t_stat stat;
    retval ret;

    ft_bzero(&stat, sizeof(t_stat));
    if (bin_fd < 0 || fstat(bin_fd, &stat) == -1)
    {
      log_error(ERR_NOFILE, bin->content);
      bin = bin->next;
      continue;
    }
    ret = __file_validate_type(&stat);
    if (ret != NO_ERR)
    {
      log_error(ret, bin->content);
      bin = bin->next;
      continue;
    }
    ret = parser_elf(bin_fd, &ctx);
    if (ret != NO_ERR)
    {
      log_error(ret, bin->content);
      bin = bin->next;
      continue ;
    }
    bin = bin->next;
  }
}
