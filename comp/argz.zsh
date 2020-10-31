_argz() {
    local -a opts
    opts=("${(@f)$(${words[@]:0:-1} help 2>/dev/null | awk '!/^#/{print $1}')}")
    case "${opts[1]}" in
        '') ;;
        CMD)  words=(${(@)words:1})
              CURRENT=1
              _normal ;;
        DIR)  _files -/ ;;
        FILE) _files ;;
        *)    compadd -a opts ;;
    esac
}
