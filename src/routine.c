#include <nm.h>

static void hexdump(void* mem, size_t n) {

}

static void TEST_get_file_magic_number(int fd)
{
  char magic[4];

  ft_memset(magic, 0, 4);
  int test = read(fd, magic, 4);
}

void routine(t_context *ctx)
{
  t_list* bin = ctx->bin;
  char* bin_path;
  int bin_fd;

  while(bin)
  {
    bin_path = (char *)bin->content;
    bin_fd = open(bin_path, O_RDONLY);
    if (bin_fd < 0)
    {
      // error mgmt
      perror("nm: ");
      exit(STDERR_FILENO);
    }
  }

}

