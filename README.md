# arg(z)

A minimal command line library, with out-of-the-box auto-completion.

Compile [readme.c](./readme.c):

    $ gcc -o readme readme.c

Enable auto-competion for `bash`:

    $ source comp/argz.bash
    $ complete -F _argz ./readme

A minimal help command is automatically generated:

    $ ./readme help
      size            a size value
      time            a time value
      percent         a custom percent value
      file            a file value

Option `size` accepts some suffixes:

    $ ./readme size 10m
    size=10485760 time=0 percent=50 file=

    $ ./readme size 10M
    size=10485760 time=0 percent=50 file=

    $ ./readme size 10MiB
    size=10485760 time=0 percent=50 file=

    $ ./readme size 10Mibytes
    size=10485760 time=0 percent=50 file=

    $ ./readme size 10MB
    size=10000000 time=0 percent=50 file=

    $ ./readme size 10MBytes
    size=10000000 time=0 percent=50 file=

    $ ./readme size 10Mb
    size=1250000 time=0 percent=50 file=

    $ ./readme size 10Mibits
    size=1310720 time=0 percent=50 file=

Option `time` too:

    $ ./readme time 10s
    size=0 time=10000 percent=50 file=

    $ ./readme time 10m
    size=0 time=600000 percent=50 file=

    $ ./readme time 10h
    size=0 time=36000000 percent=50 file=

    $ ./readme time 10d
    size=0 time=864000000 percent=50 file=

Malformed options are reported:

    $ ./readme bad
    Option bad is unknown

    $ ./readme size
    Option size requires a number

    $ ./readme size 0
    Option size must be a number greater than or equal to 1

    $ ./readme size NaN
    Option size is not a valid number

    $ ./readme time 1y
    Option time is badly suffixed

Classic input formats are supported:

    $ ./readme size -1
    size=18446744073709551615 time=0 percent=50 file=

    $ ./readme size 0xFF
    size=255 time=0 percent=50 file=

    $ ./readme size 010
    size=8 time=0 percent=50 file=
