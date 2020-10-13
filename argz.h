#pragma once

struct argz_path {
    char *path;
    int dir;
};

struct argz_ull {
    int base;
    int (*suffix)(struct argz_ull *, const char *);
    unsigned long long min, max;
    unsigned long long value;
};

struct argz {
    char *name;
    char *help;
    int (*call)(int, char **, void *);
    void *data;
    const char *const *alt;
    unsigned grp;
    int set;
};

int argz_help    (int, char **);
int argz_help_me (int, char **);
int argz_is_set  (struct argz *, const char *);

void argz_print      (struct argz *);
void argz_print_help (const char *, const char *);

int argz_time_suffix (struct argz_ull *, const char *);
int argz_size_suffix (struct argz_ull *, const char *);

int argz_path (int, char **, void *);
int argz_ull  (int, char **, void *);
int argz_str  (int, char **, void *);
int argz      (int, char **, void *);
