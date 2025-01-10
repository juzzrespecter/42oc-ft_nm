#ifndef DEBUG_H
# define DEBUG_H

# define DEBUG
# ifdef DEBUG
#    define PRINT_SECTION_VALUES(x) { \
                 printf("SECTION HEADER: \n" \
                        "\t-> index name: %d\n" \
                        "\t-> type:       %d\n" \
                        "\t-> flags:      %d\n" \
                        "\t-> address:    %p\n" \
                        "\t-> offset:     %ld\n" \
                        "\t-> link:       %d\n" \
                        "\t-> info:       %d\n" \
                        "\t-> addralign:  %d\n" \
                        "\t-> entsize:    %d\n", \
                         x->sh_name, \
                         x->sh_type, x->sh_flags, (void *)x->sh_addr, \
                         x->sh_offset, x->sh_link, x->sh_info, x->sh_addralign,x->sh_entsize); \
                }

#    define PRINT_HEADER(x) { \
                 printf("ELF HEADER: \n" \
                        "\t-> type:                                %d\n" \
                        "\t-> program header offset:               %ld\n" \
                        "\t-> section header offset:               %ld\n" \
                        "\t-> header size:                         %d\n" \
                        "\t-> program header size:                 %d\n" \
                        "\t-> number of entries in program header: %d\n" \
                        "\t-> section header size:                 %d\n" \
                        "\t-> number of entries in section header: %d\n" \
                        "\t-> index of string table:               %d\n", \
                         x->e_type ,x->e_phoff, \
                         x->e_shoff, x->e_ehsize, x->e_phentsize, \
                         x->e_phnum, x->e_shentsize, x->e_shnum, x->e_shstrndx);\
                 }
# else
#    define PRINT_SECTION_VALUES(x)
#    define PRINT_HEADER(x)
# endif


# endif // DEBUG_H