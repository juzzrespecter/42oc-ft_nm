# include "../include/nm.h"
//# include <stdint.h>

// find strtab HERE

void output_nm_symbols(t_bin *bin)
{
    t_list *node = bin->b_nm_sym_lst;

    write(STDOUT_FILENO, bin->b_src, ft_strlen(bin->b_src));
    write(STDOUT_FILENO, "\n", 1);
    for (size_t i = 0; node != NULL; i++)
    {
        t_symbol *nm_sym = (t_symbol *)node->content;

        if (nm_sym->sym_type == 'U')
        {
            if (bin->b_class == ELFCLASS32)
              write(STDOUT_FILENO, "        ", 8);
            if (bin->b_class == ELFCLASS64)
              write(STDOUT_FILENO, "                ", 16);
        }
        else {
            if (bin->b_class == ELFCLASS32)
              printf("%08lx", (unsigned long)nm_sym->sym_ptr);
            if (bin->b_class == ELFCLASS64)
              printf("%016ld", (unsigned long)nm_sym->sym_ptr);
        }
        printf(" %c %s\n",nm_sym->sym_type, nm_sym->sym_name);
        node = node->next;
    }
}