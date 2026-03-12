#include <stdio.h>

#include "cli/cli.h"
#include "files/files.h"
#include "utils/checks.h"
#include "utils/errors.h"

int show_help(void)
{
    puts("Parse and assemble `.psi` (text) files into the corresponding binary\
 code and\n\
Worldedit (https://worldedit.enginehub.org) schematic.\n\
\n\
Usage:\n\
  assembli [--verbose] [--print] [--norun] [--help] { file }\n\
\n\
Options:\n\
  --verbose:  Show extra verbose information.\n\
  --print:    Prettyprint the compiled files as they are just before exporting.\n\
  --noexport: Don't export to binary / schematic, only parse and run checks.\n\
              Useful in conjunction with --print.\n\
  --help:     Print this help message to the standard output.\n\
\n\
Examples:\n\
  assembli --noexport program.psi\n\
  assembli --verbose --print myfile.psi ../path/to/other_file.psi");
    return SUCCESS;
}

int main(int argc, char *argv[])
{
    int res = checks();
    if (res != SUCCESS)
        return res;

    struct cli_args args;
    res = parse_cli_args(argc, argv, &args);

    if (res == SUCCESS)
        res = args.help ? show_help() : process_files(&args);

    cli_args_destroy(&args);
    return res;
}
