#include "../include/nm.h"

static error __file_validate_type(t_stat* stat)
{
  if ((stat->st_mode & S_IFMT) == S_IFDIR)
  {
    return ERR_IS_DIR;
  }
  if ((stat->st_mode & S_IFMT) != S_IFREG)
    return ERR_NO_FORMAT;
  return NO_ERR;
}

/**
 * Parseamos argumentos, iniciamos la lista con los archivos a enemear.
 */
int main(int argc, char* argv[])
{
  t_nm    ctx;
  t_list* b_node;

  (void) argc;
  ft_bzero(&ctx, sizeof(ctx));
  arg_parser(&ctx, argv);

  b_node = ctx.b_lst;
  while (b_node)
  {
    t_bin* bin = b_node->content;
    t_stat stat;
    error rt;

    bin->b_fd = open(bin->b_path, O_RDONLY);
    ft_bzero(&stat, sizeof(t_stat));
    if (bin->b_fd < 0 || fstat(bin->b_fd, &stat) == -1)
    {
      log_error(ERR_SYS, bin->b_path);
      b_node = b_node->next;
      continue;
    }
    bin->b_size = stat.st_size;
    rt = __file_validate_type(&stat);
    if (rt != NO_ERR)
    {
      log_error(rt, bin->b_path);
      b_node = b_node->next;
      continue;
    }
    bin->b_src = mmap(0, stat.st_size, PROT_READ, MAP_PRIVATE, bin->b_fd, 0);
    parser_elf(bin, &ctx);
    munmap(bin->b_src, stat.st_size);
    bin->b_src = NULL;
    b_node = b_node->next;
  }
  clean_context(&ctx);
  return (EXIT_SUCCESS);
}
