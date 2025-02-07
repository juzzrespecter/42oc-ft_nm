# include "../include/nm.h"

static const t_st section_type[] =
{
  {".bss", 'b'},
  {".data", 'd'},
  {".data1", 'd'},
  {".debug", 'N'},
  {".text", 't'},
  {".fini", 't'},
  {".init", 't'},
  {".note", 'n'},
  {".rodata", 'r'},
  {".rodata1", 'r'},
  {".sdata", 'g'},
  {".sbss", 's'},
  {".scommon", 'c'}
};

/**
* Obtenemos el tipo de símbolo en base a la sección en la que se ecnuentra.
*/
static char get_nm_symbol_by_section_name(char *sh_name)
{
  for (size_t i = 0; i < sizeof(section_type) / sizeof(t_st); i++)
    {
    if (!ft_strncmp(sh_name, section_type[i].section, ft_strlen(section_type[i].section) + 1))
      return section_type[i].type;
    }
    return 0;
}

/**
* Obtenemos el tipo de símbolo en base al tipo de sección en el que se encuentra.
*/
static char get_nm_symbol_by_section_type(t_sym_info sym)
{
  if (sym.sh_type == SHT_NOBITS && sym.sh_flags & (SHF_ALLOC | SHF_WRITE))
    return 'b';
  if (sym.sh_type != SHT_NOBITS && sym.sh_flags & SHF_ALLOC && sym.sh_flags & SHF_WRITE)
    return 'd';
  if (sym.sh_type != SHT_NOBITS && !(sym.sh_flags & (SHF_WRITE|SHF_EXECINSTR)) && sym.sh_flags & SHF_ALLOC)
    return 'r';
  if (!(sym.sh_type == SHT_PROGBITS || sym.sh_type == SHT_NOBITS) && !(sym.sh_flags & SHF_WRITE))
    return 'n';
  if (sym.sh_type != SHT_NOBITS && sym.sh_flags & (SHF_ALLOC | SHF_EXECINSTR))
    return 't';
  return 0;
}

/**
* Intentamos obtener el tipo de símbolo si este no forma parte de una sección o es débil.
*/
static char get_nm_symbol_undef_weak(t_sym_info sym)
{
  if (sym.shndx == SHN_COMMON)
    return 'C';
  if (sym.shndx == SHN_UNDEF && !(sym.bind == STB_WEAK))
    return 'U';
  if (sym.shndx == SHN_UNDEF && sym.bind == STB_WEAK && sym.type == STT_OBJECT)
    return 'v';
  if (sym.shndx == SHN_UNDEF && sym.bind == STB_WEAK)
    return 'w';
  if (sym.type == STT_GNU_IFUNC)
    return 'i';
  if (sym.bind == STB_GNU_UNIQUE)
    return 'u';
  // agotamos tipos débiles
  if (sym.bind == STB_WEAK && sym.type == STT_OBJECT)
    return 'V';
  if (sym.bind == STB_WEAK)
    return 'W';
  if (!(sym.bind == STB_GLOBAL || sym.bind == STB_LOCAL))
      return '?';
  return 0;
}


/**
* Punto de entrada del análisis y obtención del tipo de símbolo del binario.
*/
char get_nm_symbol(t_sym_info sym)
{
  char c;

  c = get_nm_symbol_undef_weak(sym);
  if (c)
    return c;
  if (sym.shndx == SHN_ABS)
    return IS_GLOBAL_SYM('a', sym.bind);
  if (sym.shndx == SHN_UNDEF)
    return '?';
   c = get_nm_symbol_by_section_name(sym.sh_name);
   if (c)
     return IS_GLOBAL_SYM(c, sym.bind);
   c = get_nm_symbol_by_section_type(sym);
   if (c)
      return IS_GLOBAL_SYM(c, sym.bind);
    return '?';
}

/**
* Filtramos la creación del símbolo en base a las flags seteadas en cli.
*/
bool set_nm_visibility(char flags, t_sym_info v)
{
  if (!(flags & DEBUG_SYMS_F) && (v.type == STT_SECTION || v.type == STT_FILE))
    return false;
  if (flags & UNDEF_ONLY_F && v.shndx != SHN_UNDEF)
    return false;
  if (flags & EXTRN_ONLY_F && v.bind != STB_GLOBAL && v.bind != STB_WEAK)
    return false;
  return true;
}