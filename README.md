# arg(z)

A simple demo is worth a thousand words


```
$ cat ./test.c
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

a simple run gives:
```
$ ./test
size=1024
timeout=5000 infinity=0 auto=0
```

generated usage on bad options:
```
$ ./test help
error: `help' is unknown
usage: ./test [size BYTES]
              [timeout SECONDS
                       [infinity|auto]]
```

a simple example:
```
$ ./test size 1G
size=1073741824
timeout=5000 infinity=0 auto=0
```
```
$ ./test size NAN
error: `NAN' is not a valid BYTES for size
```

duplicate options are allowed:
```
$ ./test timeout 1s timeout infinity
size=1024
timeout=1000 infinity=1 auto=0
```

but not duplicate args:
```
$ ./test timeout 1s auto infinity
error: infinity|auto is already set to `auto'
```
