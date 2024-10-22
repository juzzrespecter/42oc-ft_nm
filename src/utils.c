# include <nm.h>

static const char *reason[] = {
  "no symbols",
  "no such file",
  "is a directory"
};

void log_error(int code, char* arg)
{
  write(STDERR_FILENO, "nm: ",4);
  write(STDERR_FILENO, arg, ft_strlen(arg));
  write(STDERR_FILENO, reason[code], ft_strlen(reason[code]));
}