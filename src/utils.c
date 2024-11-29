# include "../include/nm.h"

static const char *reason[] = {
  "no symbols",
  "no such file",
  "is a directory",
  "file format not recognized"
};

static void del_binlst(void* bin)
{
  t_bin *b = bin;

  if (b->b_fd)
    close(b->b_fd);
  if (b->b_elf_shdr)
    ft_lstclear(&b->b_elf_shdr, free);
  if (b->b_sym_lst)
    ft_lstclear(&b->b_sym_lst, free);
  free(bin);
}

int clean_context(t_nm *ctx)
{
  int program_state = EXIT_SUCCESS;

  if (ctx)
  {
    ft_lstclear(&ctx->b_lst, del_binlst);
    program_state = ctx->state;
  }
  return program_state;
}

void log_error(error code, char* arg)
{
  char buffer[50] = {0};

  if (arg)
    sprintf(buffer, "ft_nm: '%.40s'", arg);
  else
    sprintf(buffer, "ft_nm");
  if (code == ERR_SYS)
  {
    perror(buffer);
    return ;
  }
  write(STDERR_FILENO, buffer,ft_strlen(buffer));
  write(STDERR_FILENO, ": ", 2);
  write(STDERR_FILENO, reason[code], ft_strlen(reason[code]));
  write(STDERR_FILENO, "\n", 1);
}

void log_and_exit(error err, char *arg, t_nm *ctx)
{
  log_error(err, arg);
  clean_context(ctx);
  exit(EXIT_FAILURE);
}