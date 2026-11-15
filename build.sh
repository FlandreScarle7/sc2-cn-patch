#!/bin/sh
set -eu
cd "$(dirname "$0")"
LLVM="${LLVM:-$(brew --prefix llvm 2>/dev/null)/bin}"
LLD_LINK="${LLD_LINK:-$(command -v lld-link || echo "$(brew --prefix lld 2>/dev/null)/bin/lld-link")}"
mkdir -p build
"$LLVM/llvm-dlltool" -m i386:x86-64 -d src/kernel32.def -l build/kernel32.lib
"$LLVM/clang" --target=x86_64-pc-windows-msvc -O2 -ffreestanding -fno-builtin \
    -fno-stack-protector -mno-stack-arg-probe -Wall -c src/pdh.c -o build/pdh.obj
"$LLD_LINK" /dll /Brepro /machine:x64 /nodefaultlib /entry:DllMain \
    /out:build/pdh.dll build/pdh.obj build/kernel32.lib
shasum -a 256 build/pdh.dll
