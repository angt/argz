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
