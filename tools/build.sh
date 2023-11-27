#!/usr/bin/env bash

# set -x

usage() {
    local exit_code="${1:-0}"
    local program_name="$(basename "$0")"

cat<<FIN
Usage: $program_name [OPTIONS] [BUILD_DIR]

OPTIONS:
    -h,--help        Show this help
    -c,--clear       Clear DIR before build
    --cmakeargs=arg  Pass CMake argument
    -j NUMBER        Number of cores (default: 4)
FIN

    exit $exit_code
}

CORES="4"
DEFAULT_URL="https://cef-builds.spotifycdn.com/index.html"

TOP_LEVEL="$(git rev-parse --show-toplevel)"
TGT_BROWSER_ALIAS="$TOP_LEVEL/tgt"

CMAKEARGS=()

fatal() {
    echo "Fatal: $@"
    exit 1
}

os_release() {
    local attr="$1"

    if [[ -f /etc/os-release ]]; then
        echo $(egrep "^${attr}=" /etc/os-release | cut -d "=" -f 2 | tr -d '"')
    fi
}

build_dir() {
    local build_dir="build"

    if [[ -f "/etc/os-release" ]]; then
        if [[ $(os_release "ID") == "ubuntu" ]]; then
            build_dir+=".$(os_release "VERSION_CODENAME")"
        fi
    fi
    echo "$build_dir"
}

parse_args() {
    while [[ "$#" -gt 0 ]]; do
        arg="$1"; shift
        if [[ "$arg" == "-h" || "$arg" == "--help" ]]; then
            usage
        elif [[ "$arg" == "-c" || "$arg" == "--clear" ]]; then
            CLEAR=1
        elif [[ "$arg" == --cmakeargs=* ]]; then
            cmakearg=$(echo "$arg" | cut -d "=" -f 2-)
            CMAKEARGS+=($cmakearg)
        elif [[ "$arg" == "-j" ]]; then
            CORES="$1"; shift
            if [[ "$CORES" == -* ]]; then
                fatal "Syntax error at '$1'"
            fi
        else
            BUILD_DIR="$arg"
        fi
    done

    if [[ -z "$BUILD_DIR" ]]; then
        BUILD_DIR=$(build_dir)
    fi

    # CHROME_SANDBOX="$BUILD_DIR/tgt-browser/Release/chrome-sandbox"
    TGT_BROWSER="$BUILD_DIR/Release/minimal"
}

build() {
    local dirname="$1"

    if [[ -n "$CLEAR" ]]; then
        rm -Rf $dirname
    fi

    if [[ ! -d "$dirname" ]]; then
        mkdir $dirname
    fi

    cd $dirname
    cmake "${CMAKEARGS[@]}" ../
    if [[ $? -ne 0 ]]; then
        return 1
    fi
    make -j${CORES}
    cd $TOP_LEVEL

    if [[ "$?" -ne 0 ]]; then
        fatal "Build failed"
    fi
}

create_alias() {
    if [[ -e "$TGT_BROWSER" ]]; then
        rm -f "$TGT_BROWSER_ALIAS" 2>/dev/null
        ln -s "$TGT_BROWSER" "$TGT_BROWSER_ALIAS"
        print_next_step
    fi
}

print_next_step() {
    local program_name=$(realpath $TGT_BROWSER_ALIAS --relative-to $PWD)

    echo ""
    echo "Next step:"
    echo "  - $program_name --url=$DEFAULT_URL"
}

parse_args "$@"

build "$BUILD_DIR"
if [[ $? -eq 0 ]]; then
    # change_permissions
    create_alias
fi
