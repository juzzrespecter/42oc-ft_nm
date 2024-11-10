# include <nm.h>

static const char *reason[] = {
  "no symbols",
  "no such file",
  "is a directory",
  "file format not recognized"
};

void log_error(retval code, char* arg)
{
  write(STDERR_FILENO, "ft_nm: ",7);
  write(STDERR_FILENO, arg, ft_strlen(arg));
  write(STDERR_FILENO, ": ", 2);
  write(STDERR_FILENO, reason[code], ft_strlen(reason[code]));
  write(STDERR_FILENO, "\n", 1);
}