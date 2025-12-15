#ifndef CLI_H
#define CLI_H

struct cli_args {
    int run;
    int verbose;
};

int parse_cli_args(int argc, char *argv[], struct cli_args *out);

#endif /* ! CLI_H */
