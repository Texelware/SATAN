#/bin/bash
export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export ARCH=x86
export PATH="$PREFIX/bin:$PATH"

make all
