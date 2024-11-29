#include <nm.h>

static const char* valid_arguments[5][2] = {
    {"-a", "--debug-syms"},
    {"-g", "--extern-only"},
    {"-u", "--undefined-only"},
    {"-p", "--no-sort"},
    {"-r", "--reverse-sort"}
};

static const char set[5] = {
    DEBUG_SYMS_F,
    EXTRN_ONLY_F,
    UNDEF_ONLY_F,
    NO_SORT_F,
    REV_SORT_F
};

// needed clear up function
static void print_usage_and_exit()
{
    static const char* usage =
        "usage: ft_nm [option(s)] [file(s)]\n"
        "List symbols in [file(s)] (a.out by default).\n"
        "  The options are:\n"
        "     -a, --debug-syms       Display debugger-only symbols\n"
        "     -g, --extern-only      Display only external symbols\n"
        "     -u, --undefined-only   Display only undefined symbols\n"
        "     -r, --reverse-sort     Reverse the sense of the sort\n"
        "     -p, --no-sort          Do not sort the symbols\n";

    write(STDIN_FILENO, usage, ft_strlen(usage));
    exit(EXIT_FAILURE);
}

static void arg_parser_hyphen(t_nm* ctx, char* arg)
{
    int i = 0;

    for (; i < 5; i++)
    {
        const char* f_short = valid_arguments[i][0];
        const char* f_long = valid_arguments[i][1];
        const size_t len = ft_strlen(arg) + 1;

        if (!ft_strncmp(arg, f_short, len) ||
            !ft_strncmp(arg, f_long, len))
        {
            ctx->flags = ctx->flags | set[i];
            break ;
        }
    }
    if (i == 5)
    {
        clean_context(ctx);
        print_usage_and_exit();
    }
}

static t_list* generate_new_binary_node(char *bin_path)
{
    t_bin  *bin = ft_calloc(1, sizeof(t_bin));
    t_list *node;

    if (bin == NULL)
        return NULL;
    bin->b_path = bin_path;
    node = ft_lstnew(bin);
    if (node == NULL)
    {
        free(bin);
        return NULL;
    }
    return node;
}

void arg_parser(t_nm* ctx, char* argv[])
{
    t_list *b_node;

    while (*(++argv))
    {
        if (*argv[0] == '-')
        {
            arg_parser_hyphen(ctx, *argv);
            continue ;
        }
        b_node = generate_new_binary_node(*argv);
        if (b_node == NULL)
            log_and_exit(ERR_SYS, 0, ctx);
        ft_lstadd_back(&ctx->b_lst, b_node);
    }
    if (ctx->b_lst == NULL)
        ctx->b_lst = generate_new_binary_node("a.out");
}
