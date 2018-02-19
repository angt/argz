#pragma once

struct argz {
    const char *name;
    const char *kind;
    void *data;
    int (*call) (void *, int, char **);
    const char *set;
    int ret;
};

int argz_addr   (void *, int, char **);
int argz_ulong  (void *, int, char **);
int argz_uint   (void *, int, char **);
int argz_bool   (void *, int, char **);
int argz_ushort (void *, int, char **);
int argz_bytes  (void *, int, char **);
int argz_time   (void *, int, char **);
int argz_str    (void *, int, char **);
int argz_option (void *, int, char **);
int argz_is_set (struct argz *, const char *);
int argz        (struct argz *, int, char **);
