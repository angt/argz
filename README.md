# arg(z)

The minimal command line library.

```
$ cat readme.c
#include "argz.c"

int
main(int argc, char **argv)
{
    size_t size = 1024;
    unsigned long timeout = 5000;

    struct argz timeoutz[] = {
        {NULL, "SECONDS", &timeout, argz_time},
        {"infinity|auto", NULL, NULL, argz_option},
        {NULL}};

    struct argz mainz[] = {
        {"size", "BYTES", &size, argz_bytes},
        {"timeout", NULL, &timeoutz, argz_option},
        {NULL}};

    if (argz(mainz, argc, argv))
        return 1;

    printf("size=%zu\n", size);
    printf("timeout=%lu infinity=%i auto=%i\n", timeout,
           argz_is_set(timeoutz, "infinity"),
           argz_is_set(timeoutz, "auto"));

    return 0;
}
```

Run it without argument and you get

```
$ ./readme
size=1024
timeout=5000 infinity=0 auto=0
```

A minimal help command is automatically generated

```
$ ./readme help
usage: ./readme [size BYTES]
                [timeout SECONDS
                         [infinity|auto]]
```

Size and time accept suffixes

```
$ ./readme size 1G timeout 10s
size=1073741824
timeout=10000 infinity=0 auto=0
```

Bad args are reported

```
$ ./readme size NaN timeout 10s
error: `NaN' is not a valid BYTES for size
```

Duplicate options are allowed, and yes, I explicitly coded it.

```
$ ./readme timeout 1s timeout infinity
size=1024
timeout=1000 infinity=1 auto=0
```

Duplicate arguments are not allowed

```
$ ./readme timeout 1s auto infinity
error: infinity|auto is already set to `auto'
```
