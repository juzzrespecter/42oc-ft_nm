#include "../include/nm_x32.h"
#include "../include/nm.h"

static void parse_strtab(t_Elf32_Shdr *shdr, size_t strtab_index, t_bin* bin, t_nm *ctx)
{
    void     *shdr_strtab = bin->b_src + shdr->sh_offset;
    t_strtab *strtab = ft_calloc(1, sizeof(t_strtab));
    t_list   *strtab_node = ft_calloc(1, sizeof(t_list));;

    if (!strtab || !strtab_node)
        log_and_exit(ERR_SYS, NULL, ctx);
    strtab->strtab = malloc(shdr->sh_size * sizeof(char));
    if (!strtab->strtab)
    {
        free(strtab);
        free(strtab_node);
        log_and_exit(ERR_SYS, NULL, ctx);
    }
    ft_memcpy(strtab->strtab, shdr_strtab, shdr->sh_size);
    strtab->strtab_index = strtab_index;
    strtab_node->content = (void *)strtab;

    ft_lstadd_back(&bin->b_strtab_lst, strtab_node);
}

static void parse_symbol_table(t_Elf32_Shdr *shdr, t_bin* bin, t_nm *ctx)
{
    t_Elf32_Sym *sym;
    t_Elf32_Sym *sym_offset;
    t_list      *sym_node;
    size_t       n = shdr->sh_size / shdr->sh_entsize;

    sym = (t_Elf32_Sym *)((char *)bin->b_src + shdr->sh_offset);
    for (size_t i = 1; i < n; i++)
    {
        sym_offset = (t_Elf32_Sym *)((char *)sym + shdr->sh_entsize * i);
        sym_node = build_new_sym_node(sym_offset, bin->b_class, shdr->sh_link, ctx);
        ft_lstadd_back(&bin->b_sym_lst, sym_node);
    }
}

static t_Elf32_Shdr *find_section_header_x32(size_t shndx, t_bin *bin)
{
    t_list *node = bin->b_elf_shdr;
    t_Elf32_Shdr *selected_shdr = NULL;

    if (shndx == SHN_UNDEF || shndx >= SHN_LORESERVE)
        return NULL;
    for (size_t i = 0; node != NULL; i++)
    {
        if (i == shndx)
        {
            selected_shdr = (t_Elf32_Shdr *)node->content;
            break ;
        }
        node = node->next;
    }
    return selected_shdr;
}

static t_symbol *build_new_nm_symbol(t_Elf_Sym_wrapper *wr_sym, t_bin* bin, t_nm *ctx)
{
    char     *shstrtab = select_strtab(bin->shstrndx, bin);
    char     *strtab = select_strtab(wr_sym->sh_link, bin);
    char     *sh_name = NULL;
    t_symbol *ft_nm_sym;

    if (!shstrtab || !strtab)
        log_and_exit(ERR_NO_FORMAT, NULL, ctx);

    t_Elf32_Sym  *sym = wr_sym->sym;
    t_Elf32_Shdr *shdr = find_section_header_x32(sym->st_shndx, bin);
    st_bind bind = ELF64_ST_BIND(sym->st_info);
    st_type type = ELF64_ST_TYPE(sym->st_info);
    if (shdr)
        sh_name = &shstrtab[shdr->sh_name];

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
    ft_nm_sym->shndx = sym->st_shndx;
    ft_nm_sym->sym_value = (unsigned long)sym->st_value;
    ft_nm_sym->sym_type = get_nm_symbol(s);
    ft_nm_sym->sym_name = (type == STT_SECTION) ? sh_name : &strtab[sym->st_name];

    return ft_nm_sym;
}

static void parse_symbols_to_nm_fmt_x32(t_bin *bin, t_nm *ctx)
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
}

static void parser_elf_section_x32(t_bin *bin, t_nm *ctx)
{
    void   *shdr_ptr;
    t_list *shdr_node;
    ssize_t shdr_len = bin->shnum * bin->shentsize;
    size_t  i = 0;

    if (bin->shoff + shdr_len > bin->b_size)
    {
        log_error(ERR_NO_FORMAT, bin->b_path);
        return;
    }
    shdr_ptr = bin->b_src + bin->shoff;
    while (i < bin->shnum)
    {
        t_Elf32_Shdr *shdr = (t_Elf32_Shdr *)((char *)shdr_ptr + bin->shentsize * i);

        shdr_node = build_new_shdr_node(shdr, bin->b_class, ctx);
        ft_lstadd_back(&bin->b_elf_shdr, shdr_node);

        if (shdr->sh_type == SHT_STRTAB)
            parse_strtab(shdr, i, bin, ctx);
        if (shdr->sh_type == SHT_SYMTAB)
            parse_symbol_table(shdr, bin, ctx);
        i++;
    }
}

void parser_elf_hdr_x32(t_bin* bin, t_nm* ctx)
{
    t_Elf32_Hdr* hdr_ptr = bin->b_src;
    uint32_t version;

    version = ((t_Elf32_Hdr*)hdr_ptr)->e_version;
    if (version != bin->b_version)
    {
        log_error(ERR_NO_FORMAT, bin->b_path);
        return;
    }

    bin->shoff = ((t_Elf32_Hdr*)hdr_ptr)->e_shoff;
    bin->shnum = ((t_Elf32_Hdr*)hdr_ptr)->e_shnum;
    bin->shentsize = ((t_Elf32_Hdr*)hdr_ptr)->e_shentsize;
    bin->shstrndx = ((t_Elf32_Hdr*)hdr_ptr)->e_shstrndx;

    parser_elf_section_x32(bin, ctx);
    parse_symbols_to_nm_fmt_x32(bin, ctx);
}
