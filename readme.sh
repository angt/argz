gcc -o readme readme.c || exit

cmd() {
	echo \`\`\`
	echo "$ $*"
	"$@"
	echo \`\`\`
}

cat > README.md <<EOF
# arg(z)

The minimal command line library.

$(cmd cat readme.c)

Run it without argument and you get

$(cmd ./readme)

A minimal help command is automatically generated

$(cmd ./readme help)

Size and time accept suffixes

$(cmd ./readme size 1G timeout 10s)

Bad args are reported

$(cmd ./readme size NaN timeout 10s)

Duplicate options are allowed, and yes, I explicitly coded it.

$(cmd ./readme timeout 1s timeout infinity)

Duplicate arguments are not allowed

$(cmd ./readme timeout 1s auto infinity)
EOF
