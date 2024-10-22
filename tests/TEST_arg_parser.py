def TEST_no_args_ok() -> None:
    '''
    Execute command with no args.
    Returns ""
    '''
    pass

def TEST_one_bin_arg_ok() -> None:
    '''
    Execute command with one [BIN] arg.
    Returns ""
    '''
    pass

def TEST_multiple_bin_arg_ok():
    pass

def TEST_one_flag_no_bin_ok():
    pass

def TEST_one_flag_no_bin_ko():
    pass

def start() -> None:
    '''
    Test battery entrypoint.
    '''

    TEST_no_args()
    TEST_one_bin_arg()
    TEST_multiple_bin_arg()
