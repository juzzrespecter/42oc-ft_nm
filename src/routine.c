#include <nm.h>

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
printf("Hay que borrar los printfs\n");
}
}

}