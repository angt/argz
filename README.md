# arg(z)

A simple demo is worth a thousand words


```
#include "argz.c"

int main (int argc, char **argv)
{
    size_t size = 1024;
    unsigned long timeout = 5000;
    unsigned a = 1, b = 2, c = 3;

    struct argz timeoutz[] = {
        { NULL,          "SECONDS", &timeout, argz_time},
        {"infinity|auto", NULL,     NULL,     argz_option},
        {}};

    struct argz numz[] = {
        { NULL, "a", &a, argz_uint},
        { NULL, "b", &b, argz_uint},
        { NULL, "c", &c, argz_uint},
        {}};

    struct argz mainz[] = {
        {"size",   "BYTES", &size,     argz_bytes},
        {"timeout", NULL,   &timeoutz, argz_option},
        {"num",     NULL,   &numz,     argz_option},
        {}};

    if (argz(mainz, argc, argv))
        return 1;

    printf("size=%zu\n", size);
    printf("num a=%u b=%u c=%u\n", a, b, c);
    printf("timeout=%lu infinity=%i auto=%i\n", timeout,
            argz_is_set(timeoutz, "infinity"),
            argz_is_set(timeoutz, "auto"));

    return 0;
}
```

generated usage:
```
$ ./test help
error: `help' is unknown
usage: ./test [size BYTES]
              [timeout SECONDS
                       [infinity|auto]]
              [num a b c]
```

a simple example:
```
$ ./test size 1G
size=1073741824
num a=1 b=2 c=3
timeout=5000 infinity=0 auto=0

$ ./test size 1G size 5T
error: size is already set to `1G'

$ ./test size NAN
error: `NAN' is not a valid BYTES for size
```

and a more complex one:
```
$ ./test num 10 timeout auto 1d num 20 30
size=1024
num a=10 b=20 c=30
timeout=86400000 infinity=0 auto=1
```
