#include <nm.h>

/**
*  Checks the first four bytes of file,
*  returns error if these do not match to an ELF file magic number.
*/
static bool __parser_validate_magic_number(char *magic)
{
  return (magic[EI_MAG0] == ELFMAG0) &&
         (magic[EI_MAG1] == ELFMAG1) &&
         (magic[EI_MAG2] == ELFMAG2) &&
         (magic[EI_MAG3] == ELFMAG3);
}

/**
* First ELF parser step, identity part of ELF header validator.
*/
static retval __parser_identity(int fd, t_bin* bin)
{
  char    ei_ident[EI_NIDENT] = {0};
  ssize_t ret = read(fd, ei_ident, EI_NIDENT);

  if (ret < EI_NIDENT || !__parser_validate_magic_number(ei_ident))
  {
    return ERR_NOFMT;
  }
  if (ei_ident[EI_CLASS] == ELFCLASSNONE)
  {
     printf("Identificar esto\n");
  }
  if (ei_ident[EI_DATA] == ELFDATANONE)
  {
     printf("Identificar esto\n");
  }
  bin->class = ei_ident[EI_CLASS];
  bin->encoding = ei_ident[EI_DATA];
  return NO_ERR;
}

retval parser_elf(int fd, t_context* ctx)
{
  (void) ctx;
  t_bin  bin;
  retval ret;

  ft_bzero(&bin, sizeof(t_bin));
  ret = __parser_identity(fd, &bin);
  if (ret != NO_ERR)
  {
     return ret;
  }

  return NO_ERR;
  // parsear primero cabecera de archivo
  // pillar offset a la entrada de la tabla de secciones
  // leer la tabla de secciones entera
  // mientras iteramos, identificar las secciones adecuadas
  // pillar offsets a las tablas de simbolos
  // buscar tabla string
}


// function entrada para parsear cabecera de archivo

// function entrada para parsear cabecera de seccion

// function entrada para parsear simbolo