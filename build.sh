#!/bin/zsh
set -e  # stop on first error

BIN_DIR="./bin"

# ensure bin directory exists
mkdir -p "$BIN_DIR"

build_memora_cli() {
    echo "Building Memora CLI ..."
    cd memora_cli

    # Build Go binary
    go build -o "../$BIN_DIR/memora_cli" ./cmd/cli/main.go

    cd ..
    echo "Memora CLI built: $BIN_DIR/memora_cli"
}

build_memora_server() {
    echo "Building Memora server..."
    cd memora_server

    # Create build directory if missing
    mkdir -p build

    # Conan install
    conan install . \
        --output-folder=build \
        --build=missing

    cd build

    cmake .. \
        -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake \
        -DCMAKE_BUILD_TYPE=Debug

    cmake --build .

    # Copy server executable to top bin/
    cp memora_server ../../$BIN_DIR/

    cd ../..
    echo "Memora Server built: $BIN_DIR/memora_server"
}

clean_all() {
    echo "Cleaning build artifacts..."

    # Remove bin directory
    if [ -d "$BIN_DIR" ]; then
        rm -rf "$BIN_DIR"
        echo "Removed: $BIN_DIR/"
    fi

    # Remove server build directory
    if [ -d "memora_server/build" ]; then
        rm -rf "memora_server/build"
        echo "Removed: memora_server/build/"
    fi

    echo "Clean complete."
}

case "$1" in
    cli)
        build_memora_cli
        ;;
    server)
        build_memora_server
        ;;
    all)
        build_memora_cli
        build_memora_server
        ;;
    clean)
        clean_all
        ;;
    *)
        echo "Usage: ./build.sh {cli|server|all|clean}"
        exit 1
        ;;
esac
