#include <nm.h>

static void TEST_get_file_magic_number(int fd)
{
// 7f 45 4c 46
  char magic[4];

  ft_memset(magic, 0, 4);
  int test = read(fd, magic, 4);
  if (test < 4)
  {
    write(2, "boh\n", 4);
    exit(1);
  }

  printf("%d\n", (magic[3]) == 0x46);
  printf("%d\n", (magic[2]) == 0x4c);
  printf("%d\n", (magic[1]) == 0x45);
  printf("%d\n", (magic[0]) == 0x7f);

  char *hex = "0123456789abcdef";

  for (int i = 0; i < 4; i++) {
    char c1 = hex[magic[i] & 0xf];
    char c2 = hex[(magic[i] >> 4) & 0xf];

    printf("%c%c ", c2, c1);
  }
  printf("\n");
}

void routine(t_context *ctx)
{
  t_stat  file_stat;
  t_list* bin = ctx->bin;
  char*   bin_path;
  int     bin_fd;

  while(bin)
  {
    bin_path = (char *)bin->content;
    bin_fd = open(bin_path, O_RDONLY);
    if (bin_fd < 0)
    {
      // error mgmt
      perror("ft_nm: ");
      exit(STDERR_FILENO);
    }
    if (fstat(bin_fd, &file_stat) == -1)
    {
      perror("ft_nm: ");
      exit(STDERR_FILENO);
    }
// st_size para validar tamanyo, st_mode para validar tipo

    TEST_get_file_magic_number(bin_fd);
    bin = bin->next;
  }
}

