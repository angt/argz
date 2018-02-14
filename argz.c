#include "argz.h"

#include <stdarg.h>
#include <stdio.h>

static int
argz_print(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    int ret = vfprintf(stdout, fmt, ap);
    va_end(ap);

    return (ret < 0) ? 0 : ret;
}

static int
argz_cmp(const char *a, const char *s)
{
    if (!a || !s)
        return -1;

    for (size_t i = 0; a[i]; i++) {
        if (i && a[i - 1] != '|')
            continue;
        for (size_t j = 0; a[i + j] == s[j] || a[i + j] == '|'; j++)
            if (!s[j])
                return 0;
    }

    return -1;
}

static int
argz_scan_size(size_t *val, const char *s)
{
    if (!s)
        return 0;

    unsigned i = 0;

    if (s[0] && s[1])
        return -1;

    switch (s[0]) {
        default : return -1;
        case  0 : return 0;
        case 'E': i += 10; /* FALLTHRU */
        case 'P': i += 10; /* FALLTHRU */
        case 'T': i += 10; /* FALLTHRU */
        case 'G': i += 10; /* FALLTHRU */
        case 'M': i += 10; /* FALLTHRU */
        case 'K': i += 10; /* FALLTHRU */
    }

    if (val) {
        const size_t x = *val;
        const size_t y = x << i;

        if (!i || (x != (y >> i)))
            return -1;

        *val = y;
    }

    return 0;
}

static int
argz_scan_time(unsigned long *val, const char *s)
{
    if (!s)
        return 0;

    if (s[0] == 'm' && s[1] == 's' && s[2] == 0)
        return 0;

    if (s[0] && s[1])
        return -1;

    unsigned i = 1;

    switch (s[0]) {
        default : return -1;
        case 'w': i *= 7;    /* FALLTHRU */
        case 'd': i *= 24;   /* FALLTHRU */
        case 'h': i *= 60;   /* FALLTHRU */
        case 'm': i *= 60;   /* FALLTHRU */
        case  0 :            /* FALLTHRU */
        case 's': i *= 1000; /* FALLTHRU */
    }

    if (val) {
        const unsigned long x = *val;
        const unsigned long y = x * i;

        if (!i || (x != (y / i)))
            return -1;

        *val = y;
    }

    return 0;
}

static int
argz_scan_ulong(unsigned long *val, const char **end, const char *s)
{
    if (!s)
        return -1;

    unsigned long x = 0;
    int i = 0;

    if (s[i] == '+')
        i++;

    while (s[i] >= '0' && s[i] <= '9') {
        unsigned long y = 10UL * x + (s[i++] - '0');

        if (x && x >= y)
            return -1;

        x = y;
    }

    if (!i)
        return -1;

    if (end) *end = &s[i];
    if (val) *val = x;

    return 0;
}

int
argz_ulong(void *data, int argc, char **argv)
{
    if (argc < 1 || !argv[0])
        return -1;

    unsigned long val = 0;
    const char *end = NULL;
    int ret = argz_scan_ulong(&val, &end, argv[0]);

    if (ret || (end && end[0]))
        return -1;

    if (data)
        *(unsigned long *)data = val;

    return 1;
}

int
argz_uint(void *data, int argc, char **argv)
{
    if (argc < 1 || !argv[0])
        return -1;

    unsigned long tmp = 0;
    const char *end = NULL;
    int ret = argz_scan_ulong(&tmp, &end, argv[0]);
    unsigned int val = tmp;

    if (ret || (tmp != (unsigned long)val)
            || (end && end[0]))
        return -1;

    if (data)
        *(unsigned int *)data = val;

    return 1;
}

int
argz_ushort(void *data, int argc, char **argv)
{
    if (argc < 1 || !argv[0])
        return -1;

    unsigned long tmp = 0;
    const char *end = NULL;
    int ret = argz_scan_ulong(&tmp, &end, argv[0]);
    unsigned short val = tmp;

    if (ret || (tmp != (unsigned long)val)
            || (end && end[0]))
        return -1;

    if (data)
        *(unsigned short *)data = val;

    return 1;
}

int
argz_bytes(void *data, int argc, char **argv)
{
    if (argc < 1 || !argv[0])
        return -1;

    unsigned long tmp = 0;
    const char *end = NULL;
    int ret = argz_scan_ulong(&tmp, &end, argv[0]);
    size_t val = tmp;

    if (ret || (tmp != (unsigned long)val)
            || (argz_scan_size(&val, end)))
        return -1;

    if (data)
        *(size_t *)data = val;

    return 1;
}

int
argz_time(void *data, int argc, char **argv)
{
    if (argc < 1 || !argv[0])
        return -1;

    unsigned long val = 0;
    const char *end = NULL;
    int ret = argz_scan_ulong(&val, &end, argv[0]);

    if (ret || argz_scan_time(&val, end))
        return -1;

    if (data)
        *(unsigned long *)data = val;

    return 1;
}

int
argz_str(void *data, int argc, char **argv)
{
    if (argc < 1 || !argv[0])
        return -1;

    if (data)
        *(char **)data = argv[0];

    return 1;
}

int
argz_option(void *data, int argc, char **argv)
{
    if (!data)
        return 0;

    struct argz *argz = (struct argz *)data;
    int i = 0;

    for (int k = 0; i < argc && argz[k].call; k++) {
        if (argz[k].name) {
            if (argz_cmp(argz[k].name, argv[i]))
                continue;
            if (argz[k].set && (argz[k].ret >= 0)
                            && (argz[k].kind || !argz[k].data)) {
                argz[k].ret = -2;
                return -2;
            }
        } else {
            if (argz[k].ret > 0)
                continue;
        }

        int s = i + !!argz[k].name;
        int ret = argz[k].call(argz[k].data, argc - s, argv + s);

        argz[k].set = argz[k].kind ? argv[s] : argv[i];
        argz[k].ret = ret;

        if (ret <= -2)
            return -2;

        if (ret >= 0) {
            k = -1;
            i = s + ret;
        }
    }

    return i;
}

int
argz_is_set(struct argz *argz, const char *name)
{
    for (int k = 0; argz[k].call; k++) {
        if (!argz_cmp(argz[k].set, name))
            return 1;
    }

    return 0;
}

static int
argz_print_error(struct argz *argz, const char *name)
{
    for (int k = 0; argz[k].call; k++) {
        const char *n = argz[k].name ?: name;

        if (argz[k].ret == -1) {
            if (!argz[k].set) {
                argz_print("error: %s needs %s\n",
                           n, argz[k].kind ?: "an argument");
            } else {
                argz_print("error: `%s' is not a valid %s for %s\n",
                           argz[k].set, argz[k].kind ?: "argument", n);
            }
            return 1;
        }

        if (argz[k].data && argz[k].call == argz_option)
            if (argz_print_error(argz[k].data, argz[k].name))
                return 1;

        if (argz[k].ret == -2) {
            argz_print("error: %s is already set to `%s'\n", n, argz[k].set);
            return 1;
        }
    }

    return 0;
}

static int
argz_print_usage(struct argz *argz, int slen)
{
    if (!argz)
        return 0;

    int len = 0;

    for (int k = 0; argz[k].call; k++) {
        if (k && argz[k].name) {
            argz_print("\n%*s", slen, "");
            len = 0;
        }

        len += argz_print(" ");

        if (argz[k].name)
            len += argz_print("[");

        if (argz[k].name && argz[k].kind) {
            len += argz_print("%s %s", argz[k].name, argz[k].kind);
        } else {
            len += argz_print("%s", argz[k].name ?: argz[k].kind);
        }

        if (argz[k].call == argz_option)
            len += argz_print_usage(argz[k].data, slen + len);

        if (argz[k].name)
            len += argz_print("]");
    }

    return len;
}

int
argz(struct argz *argz, int argc, char **argv)
{
    if (argc >= 1) {
        int ret = argz_option(argz, argc - 1, argv + 1);

        if (ret == argc - 1)
            return 0;

        if (argz_print_error(argz, argv[0]))
            return 1;

        if (ret >= 0 && ret < argc - 1)
            argz_print("error: `%s' is unknown\n", argv[ret + 1]);
    }

    int slen = argz_print("usage: %s", argv[0]);

    if (slen > 40) {
        slen = 12;
        argz_print("\n%*s", slen, "");
    }

    argz_print_usage(argz, slen);
    argz_print("\n");

    return 1;
}