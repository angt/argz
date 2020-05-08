#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "argz.h"

int
argz_help(int argc, char **argv)
{
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "help"))
            return 1;
    }
    return 0;
}

int argz_help_me(int argc, char **argv)
{
    return argz_help(argc > 2 ? 2 : argc, argv);
}

static int
argz_cmp(struct argz *z, char *name)
{
    if (!strcmp(z->name, name))
        return 0;
    if (z->alt) for (unsigned k = 0; z->alt[k]; k++) {
        if (!strcmp(z->alt[k], name))
            return 0;
    }
    return 1;
}

static int
argz_is_available(struct argz *z, unsigned i, unsigned *ret)
{
    if (z[i].set)
        return 0;
    if (z[i].grp) for (unsigned k = 0; z[k].name; k++) {
        if (z[k].set && z[k].grp == z[i].grp) {
            if (ret) *ret = k;
            return 0;
        }
    }
    return 1;
}

void
argz_print(struct argz *z)
{
    int len = 0;

    for (int i = 0; z[i].name; i++) {
        if (!argz_is_available(z, i, NULL))
            continue;
        int nlen = strlen(z[i].name);
        if (len < nlen)
            len = nlen;
    }
    for (int i = 0; z[i].name; i++) {
        if (!argz_is_available(z, i, NULL))
            continue;
        printf("    %-*s    %s\n", len, z[i].name, z[i].help ? z[i].help : "");
    }
}

int
argz(int argc, char **argv, void *data)
{
    struct argz *z = (struct argz *)data;
    int a = 1;

    while (a < argc) {
        if (!strcmp(argv[a], "help")) {
            argz_print(z);
            return 0;
        }
        int set = 0;
        for (unsigned i = 0; z[i].name; i++) {
            if (argz_cmp(&z[i], argv[a]))
                continue;
            unsigned k = 0;
            if (!argz_is_available(z, i, &k)) {
                fprintf(stderr, "Incompatibility between %s and %s\n",
                        z[i].name, z[k].name);
                return 0;
            }
            z[i].set = set = 1;
            a = z[i].call ? argc - z[i].call(argc - a, argv + a, z[i].data)
                          : a + 1;
            break;
        }
        if (!set) {
            fprintf(stderr, "Unknown: %s\n", argv[1]);
            return 0;
        }
    }
    return argc - a;
}
