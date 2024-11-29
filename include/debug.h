#ifndef DEBUG_H
# define DEBUG_H

# define DEBUG
# ifdef DEBUG
#    define PRINT_SECTION_VALUES(x) {
                 printf("SECTION HEADER: ");
                 printf("\t-> index name: %d\n", x->sh_name);
                 printf("\t-> type:       %d\n", x->sh_type);
                 printf("\t-> flags:      %d\n", x->sh_flags);
                 printf("\t-> address:    %p\n", x->sh_addr);
                 printf("\t-> offset:     %d\n", x->sh_offset);
                 printf("\t-> link:       %d\n", x->sh_link);
                 printf("\t-> info:       %d\n", x->sh_info);
                 printf("\t-> addralign:  %d\n", x->sh_addalign);
                 printf("\t-> entsize:    %d\n", x->sh_entsize);

#    define PRINT_HEADER(x) {
                 printf("ELF HEADER: ");
                 print("\t-> type: %d\n", x->e_type);
                 printf("\t-> program header offset: %d", x->e_phoff;
                 printf("\t-> section header offset: %d ", x->e_shoff;
                 printf("\t-> header size: %d ", x->e_ehsize;
                 printf("\t-> program header size: %d ", x->e_phentsize;
                 printf("\t-> number of entries in program header: %d ", x->e_phnum;
                 printf("\t-> section header size: %d ", x->e_shentsize;
                 printf("\t-> number of entries in section header: %d ", x->e_shnum;
                 printf("\t-> index of string table: %d ", x->e_shstrndx;
# else
#    define PRINT_SECTION_VALUES(x)
#    define PRINT_HEADER(x)
# endif


# endif // DEBUG_H