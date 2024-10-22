#ifndef NM_H
# define NM_H

# include <libft.h>
# include <fcntl.h>
# include <unistd.h>
# include <stdio.h> // to remove

# define DEBUG_SYMS_F 0x00001
# define EXTRN_ONLY_F 0x00002
# define UNDEF_ONLY_F 0x00004
# define NO_SORT_F    0x00008
# define REV_SORT_F   0x00010

typedef struct s_context
{
  t_list *bin;
  char    flags;
} t_context;

void arg_parser(t_context*, int, char**);
void elf_parser(t_context*);

int loader(t_context*);

#endif //NM_H