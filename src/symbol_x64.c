#include "../include/nm.h"
#include "../include/nm_x64.h"

static t_symbol *build_new_nm_symbol(t_Elf_Sym_wrapper *wr_sym, t_bin* bin, t_nm *ctx)
{
    char     *shstrtab = select_strtab(bin->shstrndx, bin);
    char     *strtab = select_strtab(wr_sym->sh_link, bin);
    char     *sh_name = NULL;
    t_symbol *ft_nm_sym;

    if (!shstrtab || !strtab)
        log_and_exit(ERR_NO_FORMAT, NULL, ctx);

    t_Elf64_Sym  *sym = wr_sym->sym;
    t_Elf64_Shdr *shdr = find_section_header_x64(sym->st_shndx, bin);
    st_bind bind = ELF64_ST_BIND(sym->st_info);
    st_type type = ELF64_ST_TYPE(sym->st_info);
    if (shdr)
        sh_name = &shstrtab[shdr->sh_link];

    t_sym_info s = {
        .type = type,
        .bind = bind,
        .shndx = sym->st_shndx,
        .sh_name = sh_name,
        .sh_type = (shdr) ? shdr->sh_type : 0,
        .sh_flags = (shdr) ? shdr->sh_flags: 0,
    };
    if (!set_nm_visibility(ctx->flags, s))
        return NULL;
    ft_nm_sym = ft_calloc(1, sizeof(t_symbol));
    if (!ft_nm_sym)
        log_and_exit(ERR_SYS, NULL, ctx);
    ft_nm_sym->sym_value = (void *)sym->st_value;
    ft_nm_sym->sym_type = get_nm_symbol(s);
    ft_nm_sym->sym_name = &strtab[sym->st_name];

    return ft_nm_sym;
}

void parse_symbols_to_nm_fmt_x64(t_bin *bin, t_nm *ctx)
   {
   t_list   *node = bin->b_sym_lst;
   t_symbol *nm_sym;
   t_list   *nm_sym_node;

   for (; node != NULL; node = node->next)
   {
       nm_sym = build_new_nm_symbol((t_Elf_Sym_wrapper *)node->content, bin, ctx);
       if (!nm_sym)
           continue ;
       nm_sym_node = ft_lstnew(nm_sym);
       ft_lstadd_back(&bin->b_nm_sym_lst, nm_sym_node);
   }
    output_nm_symbols(bin, ctx);
#ifdef _NM_DEBUG
    printf("asdjashkdhaskjdha\n");
#endif
  }
