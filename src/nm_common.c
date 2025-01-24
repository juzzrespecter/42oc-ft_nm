# include "../include/nm.h"
//# include <stdint.h>

// find strtab HERE

void output_nm_symbols(t_bin *bin)
{
    t_list *node = bin->b_nm_sym_lst;

    for (size_t i = 0; node != NULL; i++)
    {
        t_symbol *nm_sym = (t_symbol *)node->content;

        if (bin->b_class == ELFCLASS32)
          printf("%08ld", (unsigned long)nm_sym->sym_ptr);
        if (bin->b_class == ELFCLASS64)
          printf("%016ld", (unsigned long)nm_sym->sym_ptr);
        printf(" %c %s\n",nm_sym->sym_type, nm_sym->sym_name);
        node = node->next;
    }
}