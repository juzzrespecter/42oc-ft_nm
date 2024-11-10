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

static void arg_parser_hyphen(t_context* ctx, char *arg)
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
        // print error
        print_usage_and_exit();
    }
}

void arg_parser(t_context* ctx, int argc, char* argv[])
{
    if (argc < 2)
    {
        ctx->bin = ft_lstnew("a.out");
        return ;
    }
    while (*(++argv))
    {
      if (*argv[0] == '-') {
        arg_parser_hyphen(ctx, *argv);
        continue ;
      }
      ft_lstadd_back(&ctx->bin, ft_lstnew(*argv));
      if (ctx->bin == NULL)
      {
          ctx->bin = ft_lstnew("a.out");
      }
    }
}