#define _GNU_SOURCE

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "argz.h"

#ifndef ARGZ_LEN
#define ARGZ_LEN 10
#endif

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
argz_cmp(struct argz *z, const char *name)
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

int
argz_is_set(struct argz *z, const char *name)
{
    if (z) for (unsigned i = 0; z[i].name; i++) {
        if (argz_cmp(&z[i], name))
            continue;
        return z[i].set;
    }
    return 0;
}

void
argz_print_help(const char *name, const char *help)
{
    printf("    %-*s    %s\n", ARGZ_LEN, name, help ? help : "");
}

void
argz_print(struct argz *z)
{
    if (z) for (int i = 0; z[i].name; i++) {
        if (!argz_is_available(z, i, NULL))
            continue;
        argz_print_help(z[i].name, z[i].help);
    }
}

int
argz_str(int argc, char **argv, void *data)
{
    if (argz_help_me(argc, argv)) {
        const char *str = *(const char **)data;
        if (str && *str)
            printf("%s\n", str);
    } else if (argc > 1) {
        memcpy(data, &argv[1], sizeof(char *));
        return argc - 2;
    } else {
        fprintf(stderr, "Option %s requires a value\n", argv[0]);
    }
    return -1;
}

int
argz_path(int argc, char **argv, void *data)
{
    struct argz_path *z = (struct argz_path *)data;

    if (argz_help_me(argc, argv)) {
        if (z->dir) {
            printf("DIR\n");
        } else {
            printf("FILE\n");
        }
    } else if (argc > 1) {
        z->path = argv[1];
        return argc - 2;
    } else {
        fprintf(stderr, "Option %s requires a path\n", argv[0]);
    }
    return -1;
}

int
argz_ull(int argc, char **argv, void *data)
{
    struct argz_ull *z = (struct argz_ull *)data;

    if (argz_help_me(argc, argv)) {
        unsigned long long value = z->value;
        if (z->min && (z->min > z->value))
            value = z->min;
        if (z->max && (z->max < z->value))
            value = z->max;
        printf("%llu\n", value);
    } else if (argc > 1) {
        char *end = NULL;
        z->value = (errno = 0, strtoull(argv[1], &end, z->base));
        if (errno == ERANGE) {
            fprintf(stderr, "Option %s is out of range\n", argv[0]);
            return -1;
        }
        if (errno || end == argv[1]) {
            fprintf(stderr, "Option %s is not a valid number\n", argv[0]);
            return -1;
        }
        if ((z->suffix && z->suffix(z, end)) || (!z->suffix && end && *end)) {
            fprintf(stderr, "Option %s is badly suffixed\n", argv[0]);
            return -1;
        }
        if (z->min && (z->value < z->min)) {
            fprintf(stderr, "Option %s must be a number greater than or equal to %llu\n",
                    argv[0], z->min);
            return -1;
        }
        if (z->max && (z->value > z->max)) {
            fprintf(stderr, "Option %s must be a number less than or equal to %llu\n",
                    argv[0], z->max);
            return -1;
        }
        return argc - 2;
    } else {
        fprintf(stderr, "Option %s requires a number\n", argv[0]);
    }
    return -1;
}

int
argz_time_suffix(struct argz_ull *ull, const char *s)
{
    if (!s)
        return 0;

    if (!strcmp(s, "ms"))
        return 0;

    if (s[0] && s[1])
        return -1;

    unsigned long long i = 1;

    switch (s[0]) {
        default : return -1;
        case 'w': i *= 7;    /* FALLTHRU */
        case 'd': i *= 24;   /* FALLTHRU */
        case 'h': i *= 60;   /* FALLTHRU */
        case 'm': i *= 60;   /* FALLTHRU */
        case  0 :            /* FALLTHRU */
        case 's': i *= 1000; /* FALLTHRU */
    }
    if (!i || ull->value > ULLONG_MAX / i)
        return -1;

    ull->value *= i;
    return 0;
}

int
argz_size_suffix(struct argz_ull *ull, const char *s)
{
    if (!s)
        return 0;

    unsigned n = 0;

    switch (s[0]) {
        case 'g':           /* FALLTHRU */
        case 'G': n++;      /* FALLTHRU */
        case 'm':           /* FALLTHRU */
        case 'M': n++;      /* FALLTHRU */
        case 'k':           /* FALLTHRU */
        case 'K': n++; s++; /* FALLTHRU */
    }

    unsigned long long c = 1024;
    unsigned long long i = 1;

    if (s[0] == 'i') {
        s++;
    } else if (s[0]) {
        c = 1000;
    }
    while (n--)
        i *= c;

    if (!strcmp(s, "b") || !strcasecmp(s, "bit")
                        || !strcasecmp(s, "bits")) {
        i >>= 3;
    } else if (s[0] && strcmp(s, "B") && strcasecmp(s, "byte")
                                      && strcasecmp(s, "bytes")) {
        return -1;
    }
    if (!i || ull->value > ULLONG_MAX / i)
        return -1;

    ull->value *= i;
    return 0;
}

int
argz(int argc, char **argv, void *data)
{
    struct argz *z = (struct argz *)data;
    int a = 1;

    while (a < argc) {
        if (!strcmp(argv[a], "help")) {
            argz_print(z);
            return -1;
        }
        int set = 0;
        if (z) for (unsigned i = 0; z[i].name; i++) {
            if (argz_cmp(&z[i], argv[a]))
                continue;
            unsigned k = 0;
            if (!argz_is_available(z, i, &k)) {
                fprintf(stderr, "Option %s is not compatible with %s\n",
                        z[i].name, z[k].name);
                return -1;
            }
            z[i].set = set = 1;
            int ret = z[i].call ? z[i].call(argc - a, argv + a, z[i].data)
                                : argc - a - 1;
            if (ret < 0) {
                if (z[i].call == argz)
                    argz_print(z);
                return ret;
            }
            a = argc - ret;
            break;
        }
        if (!set)
            break;
    }
    return argc - a;
}

int
argz_main(int argc, char **argv, struct argz *z)
{
    int ret = argz(argc, argv, z);

    if (ret > 0 && argc > ret) {
        fprintf(stderr, "Option %s is unknown\n", argv[argc - ret]);
        return -1;
    }
    return ret;
}
