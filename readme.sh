cmd() {
	( echo "$ $*"; "$@" 2>&1) | sed 's/^/    /'
}

cat > README.md <<EOF
# arg(z)

A minimal command line library, with out-of-the-box auto-completion.

Compile [readme.c](./readme.c):

$(cmd gcc -o readme readme.c)

Enable auto-competion for \`bash\`:

    $ source comp/argz.bash
    $ complete -F _argz ./readme

A minimal help command is automatically generated:

$(cmd ./readme help)

Option \`size\` accepts some suffixes:

$(cmd ./readme size 10m)

$(cmd ./readme size 10M)

$(cmd ./readme size 10MiB)

$(cmd ./readme size 10Mibytes)

$(cmd ./readme size 10MB)

$(cmd ./readme size 10MBytes)

$(cmd ./readme size 10Mb)

$(cmd ./readme size 10Mibits)

Option \`time\` too:

$(cmd ./readme time 10s)

$(cmd ./readme time 10m)

$(cmd ./readme time 10h)

$(cmd ./readme time 10d)

Malformed options are reported:

$(cmd ./readme bad)

$(cmd ./readme size)

$(cmd ./readme size 0)

$(cmd ./readme size NaN)

$(cmd ./readme time 1y)

Classic input formats are supported:

$(cmd ./readme size -1)

$(cmd ./readme size 0xFF)

$(cmd ./readme size 010)
EOF
