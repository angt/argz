#include "argz.c"

int
readme_percent_suffix(struct argz_ull *ull, const char *s)
{
    return s && s[0] && strcmp(s, "%");
}

int
main(int argc, char **argv)
{
    struct argz_ull size = {
        .min = 1,
        .suffix = argz_size_suffix,
    };
    struct argz_ull time = {
        .suffix = argz_time_suffix,
    };
    struct argz_ull percent = {
        .min = 0, .max = 100, .value = 50,
        .suffix = readme_percent_suffix,
    };
    struct argz_path file = {0};
    struct argz z[] = {
        {"size" , "a size value", argz_ull, &size},
        {"time" , "a time value", argz_ull, &time},
        {"percent", "a custom percent value", argz_ull, &percent},
        {"file", "a file value", argz_path, &file},
        {0}};

    int err = argz_main(argc, argv, z);

    if (err)
        return err;

    printf("size=%llu time=%llu percent=%llu file=%s\n",
            size.value, time.value, percent.value, file.path ? file.path : "");

    return 0;
}
