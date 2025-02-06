# ifdef _DEBUG_NM
# include "../include/nm.h"
# include "../include/nm_x32.h"
# include "../include/nm_x64.h"

void TEST_print_symbols(t_bin *b)
{
    t_list *n = b->b_sym_lst;

    for (size_t i = 0; n != NULL; i++)
    {
        print_symbol_table_x64((t_Elf_Sym_wrapper *)n->content, b, i);
        n = n->next;
    }
}

void TEST_print_sections(t_bin *b)
{
    t_list *n = b->b_elf_shdr;

    for (int i = 0; n != NULL; i++)
    {
        print_section_values_x64((t_Elf64_Shdr *)n->content, b, i);
        n = n->next;
    }
}

# endif