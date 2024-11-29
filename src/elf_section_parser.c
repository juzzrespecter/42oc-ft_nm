#include "../include/nm.h"

t_list *build_new_shdr_node(t_Elf64_Shdr *shdr, t_nm *ctx)
{
  void   *shdr_content = malloc(sizeof(t_Elf64_Shdr));
  t_list *shdr_node;

  if (shdr_content == NULL)
    log_and_exit(ERR_SYS, NULL, ctx);
  ft_memcpy(shdr_content, shdr, sizeof(t_Elf64_Shdr));
  shdr_node = ft_lstnew(shdr_content);
  if (shdr_node == NULL)
  {
    free(shdr_content);
    log_and_exit(ERR_SYS, NULL, ctx);
  }
  return shdr_node;
}

void parser_elf_section(t_bin *bin, t_nm *ctx)
{
  void   *shdr_ptr;
  t_list *shdr_node;
  ssize_t shdr_len = bin->shnum * bin->shentsize;
  off_t   pa_shoff = bin->shoff & ~(sysconf(_SC_PAGESIZE) - 1);
  int     i = 0;

  if (bin->shoff + shdr_len > bin->b_size)
  {
     log_error(ERR_NO_FORMAT, bin->b_path);
     return;
  }
  printf("PA_OFFSET: %ld, OFFSET: %ld\n", pa_shoff, bin->shoff);
  shdr_ptr = mmap((void *)0, shdr_len + pa_shoff, PROT_WRITE, MAP_PRIVATE, bin->b_fd, pa_shoff);
  if (shdr_ptr == MAP_FAILED)
    log_and_exit(ERR_SYS, NULL, ctx);

  pa_shoff = bin->shoff - pa_shoff;
  while (i < bin->shnum)
  {
    t_Elf64_Shdr *shdr = (t_Elf64_Shdr *)(shdr_ptr) + pa_shoff;

    shdr_node = build_new_shdr_node(shdr, ctx);
    ft_lstadd_back(&bin->b_elf_shdr, shdr_node);

    PRINT_SECTION_VALUES(shdr);
    pa_shoff += (bin->shentsize);
  }
  printf("Buenos dias\n");
}
