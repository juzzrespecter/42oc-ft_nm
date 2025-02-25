#include "../include/nm.h"

static t_bin *validate_type(t_bin* bin, t_nm *ctx)
{
  t_stat stat;

  bin->b_fd = open(bin->b_path, O_RDONLY);
  if (bin->b_fd < 0 || fstat(bin->b_fd, &stat) < 0)
    return log_error(ERR_SYS, bin->b_path, ctx);
  if ((stat.st_mode & S_IFMT) == S_IFDIR)
    return log_error(ERR_IS_DIR, bin->b_path, ctx);
  if ((stat.st_mode & S_IFMT) != S_IFREG)
  {
    if ((stat.st_mode & S_IFMT) == S_IFDIR)
      return log_error(ERR_IS_DIR, bin->b_path, ctx);
    return log_error(ERR_BAD_FILE, bin->b_path, ctx);
  }
  bin->b_size = stat.st_size;
  return bin;
}

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

    bin = validate_type(bin, &ctx);
    if (bin == NULL)
    {
      b_node = b_node->next;
      continue;
    }
    bin->b_src = mmap(0, bin->b_size, PROT_READ, MAP_PRIVATE, bin->b_fd, 0);
    parser_elf(bin, &ctx);
    munmap(bin->b_src, bin->b_size);
    bin->b_src = NULL;
    b_node = b_node->next;
  }
  return clean_context(&ctx);
}