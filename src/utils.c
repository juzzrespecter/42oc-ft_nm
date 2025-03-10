# include "../include/nm.h"
# include "../include/nm_x64.h"
# include "../include/nm_x32.h"

static const char* reason[] = {
  "no symbols",
  "no such file",
  "is a directory",
  "file format not recognized",
  "is not an ordinary file"
};

static void del_strtab(void *ptr)
{
  t_strtab* tab = (t_strtab*)ptr;

  if (tab)
    free(tab->strtab);
  free(tab);
}

static void del_symlist(void *ptr)
{
  t_Elf_Sym_wrapper* sym_wrapper = (t_Elf_Sym_wrapper*)ptr;

  if (sym_wrapper)
    free(sym_wrapper->sym);
  free(ptr);
}

static void del_binlst(void* bin)
{
  t_bin* b = bin;

  if (b->b_fd)
    close(b->b_fd);
  if (b->b_elf_shdr)
    ft_lstclear(&b->b_elf_shdr, free);
  if (b->b_sym_lst)
    ft_lstclear(&b->b_sym_lst, del_symlist);
  if (b->b_strtab_lst)
    ft_lstclear(&b->b_strtab_lst, del_strtab);
  if (b->b_nm_sym_lst)
    ft_lstclear(&b->b_nm_sym_lst, free);
  if (b->b_src)
    munmap(b->b_src, b->b_size);
  free(bin);
}

int clean_context(t_nm* ctx)
{
  int program_state = EXIT_SUCCESS;

  if (ctx)
  {
    if (ctx->b_lst)
      ft_lstclear(&ctx->b_lst, del_binlst);
    program_state = ctx->state;
  }
  return program_state;
}

static void *log_generate_buffer(char *arg)
{
  char  *buffer;
  size_t buffer_len;

  if (arg)
    buffer_len = snprintf(NULL, 0, "ft_nm: '%s'", arg);
  else
    buffer_len = snprintf(NULL, 0, "ft_nm");
  buffer = malloc((buffer_len + 1) * sizeof(char));
  if (!buffer)
    return NULL;
  if (arg)
    sprintf(buffer, "ft_nm: '%s'", arg);
  else
    sprintf(buffer, "ft_nm");
  return buffer;
}

void log_info(error code, t_bin *bin, t_nm* ctx)
{
  char  *buffer = log_generate_buffer(bin->b_path);

  if (!buffer)
  {
    perror("malloc");
    clean_context(ctx);
    exit(EXIT_FAILURE);
  }
  if (ctx->b_lst->next)
    printf("\n%s:\n", bin->b_path);
  dprintf(STDOUT_FILENO, "%s: %s\n", buffer, reason[code]);
  free(buffer);
}

void *log_error(error code, char* arg, t_nm* ctx)
{
  char  *buffer = log_generate_buffer(arg);

  ctx->state = 1;
  if (!buffer)
  {
    perror("malloc");
    clean_context(ctx);
    exit(EXIT_FAILURE);
  }
  if (code == ERR_SYS)
    perror(buffer);
  else
    dprintf(STDERR_FILENO, "%s: %s\n", buffer, reason[code]);
  free(buffer);
  return NULL;
}

void log_and_exit(error err, char* arg, t_nm* ctx)
{
  log_error(err, arg, ctx);
  clean_context(ctx);
  exit(EXIT_FAILURE);
}

/* Debug utils */

void print_elf_header_x64(t_Elf64_Hdr *h)
{
  char *e_type;

  printf("ELF HEADER:\n");
  switch (h->e_type)
  {
    case ET_NONE: e_type = "ET_NONE"; break;
    case ET_REL: e_type = "ET_REL"; break;
    case ET_EXEC: e_type = "ET_EXEC"; break;
    case ET_DYN: e_type = "ET_DYN"; break;
    case ET_CORE: e_type = "ET_CORE"; break;
    case ET_LOPROC: e_type = "ET_LOPROC"; break;
    case ET_HIPROC: e_type = "ET_HIPROC"; break;
    default: e_type = "unknown"; break;
  }
  printf("\t· e_type: %s\n", e_type);

}

void print_section_values_x64(t_Elf64_Shdr* s, t_bin *b, int i)
{
  char *strtab = select_strtab(b->shstrndx, b);
  char *sh_name;

  if (strtab)
    sh_name = &strtab[s->sh_name];
  else
    sh_name = "[ could not retrieve string table ]";

  printf("SECTION HEADER (%d)\n", i);
  printf("\t· sh_name (Index name): %d (%s)\n", s->sh_name, sh_name);
  printf("\t· sh_type (Index type): ");
  switch (s->sh_type)
  {
  case SHT_NULL: printf("SHT_NULL\n");                   break;
  case SHT_PROGBITS: printf("SHT_PROGBITS\n");           break;
  case SHT_SYMTAB: printf("SHT_SYMTAB\n");               break;
  case SHT_STRTAB: printf("SHT_STRTAB\n");               break;
  case SHT_RELA: printf("SHT_RELA\n");                   break;
  case SHT_HASH: printf("SHT_HASH\n");                   break;
  case SHT_DYNAMIC: printf("SHT_DYNAMIC\n");             break;
  case SHT_NOTE: printf("SHT_NOTE\n");                   break;
  case SHT_NOBITS: printf("SHT_NOBITS\n");               break;
  case SHT_REL: printf("SHT_REL\n");                     break;
  case SHT_SHLIB: printf("SHT_SHLIB\n");                 break;
  case SHT_DYNSYM: printf("SHT_DYNSYM\n");               break;
  case SHT_INIT_ARRAY: printf("SHT_INIT_ARRAY\n");       break;
  case SHT_FINI_ARRAY: printf("SHT_FINI_ARRAY\n");       break;
  case SHT_PREINIT_ARRAY: printf("SHT_PREINIT_ARRAY\n"); break;
  case SHT_GROUP: printf("SHT_GROUP\n");                 break;
  case SHT_SYMTAB_SHNDX: printf("SHT_SYMTAB_SHNDX\n");   break;
  default:
    if (s->sh_type >= SHT_LOPROC && s->sh_type <= SHT_HIPROC)
      printf("[ Processor specific ]\n");
    if (s->sh_type >= SHT_LOUSER && s->sh_type <= SHT_HIUSER)
      printf("[ Application specific ]\n");
  }
  printf("\t· sh_flags (section flags): ");
  if (s->sh_flags & SHF_WRITE)            printf("SHF_WRITE ");
  if (s->sh_flags & SHF_ALLOC)            printf("SHF_ALLOC ");
  if (s->sh_flags & SHF_EXECINSTR)        printf("SHF_EXECINSTR ");
  if (s->sh_flags & SHF_MERGE)            printf("SHF_MERGE ");
  if (s->sh_flags & SHF_STRING)           printf("SHF_STRING ");
  if (s->sh_flags & SHF_INFO_LINK)        printf("SHF_INFO_LINK ");
  if (s->sh_flags & SHF_LINK_ORDER)       printf("SHF_LINK_ORDER ");
  if (s->sh_flags & SHF_OS_NONCONFORMING) printf("SHF_OS_NONCONFORMING ");
  if (s->sh_flags & SHF_GROUP)            printf("SHF_GROUP ");
  if (s->sh_flags & SHF_TLS)              printf("SHF_TLS ");
  if (s->sh_flags & SHF_MASKOS)           printf("SHF_MASKOS ");
  if (s->sh_flags & SHF_MASKPROC)         printf("SHF_MASKPROC ");
  printf("\n");
  printf("\t· sh_addr (section  address): %p\n", (void*)s->sh_addr);
  printf("\t· sh_offset (section offset): %ld\n", s->sh_offset);
  printf("\t· sh_size (section size): %ld\n", s->sh_size);
  printf("\t· sh_link (section link): %d\n", s->sh_link);
  printf("\t· sh_info (section info): %d\n", s->sh_info);
  printf("\t· sh_addralign (section address): %ld\n", s->sh_addralign);
  printf("\t· sh_entsize (section entry size): %ld\n\n\n", s->sh_entsize);
}

void print_symbol_table_x64(t_Elf_Sym_wrapper* n, t_bin* b, int i)
{
  t_Elf64_Sym  *s = (t_Elf64_Sym *)n->sym;
  t_Elf64_Shdr *shdr = find_section_header_x64(s->st_shndx, b);

  char *st_type, *st_bind, *st_name, *sh_name;
  char *strtab = select_strtab(n->sh_link, b);
  char *shstrtab = (shdr) ? select_strtab(b->shstrndx, b) : NULL;

  st_name = (strtab != NULL) ? &strtab[s->st_name] : "[ unknown ]";
  sh_name = (shstrtab != NULL) ? &shstrtab[shdr->sh_name] : "[ unlisted ]";

  switch (ELF64_ST_TYPE(s->st_info))
  {
    case (STT_NOTYPE): st_type = "STT_NOTYPE" ; break;
    case (STT_OBJECT): st_type = "STT_OBJECT" ; break;
    case (STT_FUNC): st_type = "STT_FUNC" ; break;
    case (STT_SECTION): st_type = "STT_SECTION" ; break;
    case (STT_FILE): st_type = "STT_FILE" ; break;
    case (STT_COMMON): st_type = "STT_COMMON" ; break;
    case (STT_TLS): st_type = "STT_TLS" ; break;
    case (STT_LOOS): st_type = "STT_LOOS" ; break;
    case (STT_HIOS): st_type = "STT_HIOS" ; break;
    case (STT_LOPROC): st_type = "STT_LOPROC" ; break;
    case (STT_HIPROC): st_type = "STT_HIPROC" ; break;
    default: st_type = "[ unknown ]" ; break;
  }

  switch (ELF64_ST_BIND(s->st_info))
  {
    case (STB_LOCAL): st_bind = "STB_LOCAL"; break;
    case (STB_GLOBAL): st_bind = "STB_GLOBAL"; break;
    case (STB_WEAK): st_bind = "STB_WEAK"; break;
    case (STB_LOOS): st_bind = "STB_LOOS"; break;
    case (STB_HIOS): st_bind = "STB_HIOS"; break;
    case (STB_LOPROC): st_bind = "STB_LOPROC"; break;
    case (STB_HIPROC): st_bind = "STB_HIPROC"; break;
    default: st_bind = "[ unknown ]" ; break;
  }

  printf("SYMBOL (%d)\n", i);
  printf("\t· st_name: %d -> %s\n", s->st_name, st_name);
  printf("\t· st_value (Symbol value): %p\n", (void *)s->st_value);
  printf("\t· st_size: %ld\n", s->st_size);
  printf("\t· st_info: %d (%s, %s)\n", s->st_info, st_type, st_bind);
  printf("\t· st_shndx: %d -> %s\n", s->st_shndx, sh_name);
  printf("\t· st_other: %d\n", s->st_other);
}
