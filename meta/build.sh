#!/bin/bash

set -xe

CC="ccache clang"
CFLAGS="-Wno-excessive-regsave -m32 -mno-mmx -mno-sse -mno-sse2 -ffreestanding --target=i386-freestanding -std=c23 -Wall -Wextra -pedantic -Werror -Wno-gnu-zero-variadic-macro-arguments -Isrc -O2 -nostdlib -mstack-alignment=8"

mkdir -p build
mkdir -p build/core
mkdir -p build/hardware
mkdir -p build/kernel
mkdir -p build/prekernel
echo '*' > build/.gitignore

$CC -c $CFLAGS src/core/arith64.c -o build/core/arith64.o
$CC -c $CFLAGS src/core/kprintf.c -o build/core/kprintf.o -Wno-unused-function

$CC -c $CFLAGS src/hardware/acpi.c      -o build/hardware/acpi.o
$CC -c $CFLAGS src/hardware/interrupt.c -o build/hardware/interrupt.o
$CC -c $CFLAGS src/hardware/pci.c       -o build/hardware/pci.o
$CC -c $CFLAGS src/hardware/serial.c    -o build/hardware/serial.o

$CC -c $CFLAGS src/kernel/kernel.c    -o build/kernel/kernel.o
$CC -c $CFLAGS src/kernel/multiboot.c -o build/kernel/multiboot.o

$CC -c $CFLAGS src/main.c -o build/main.o

nasm -felf32 src/prekernel/multiboot_header.nasm -o build/prekernel/multiboot_header.o
nasm -felf32 src/prekernel/start.nasm            -o build/prekernel/start.o

ld.lld \
    -m elf_i386 \
    --nmagic \
    -T meta/linker.ld \
    -o kernel \
    build/core/arith64.o \
    build/core/kprintf.o \
    build/hardware/acpi.o \
    build/hardware/interrupt.o \
    build/hardware/pci.o \
    build/hardware/serial.o \
    build/kernel/kernel.o \
    build/kernel/multiboot.o \
    build/prekernel/multiboot_header.o \
    build/prekernel/start.o \
    build/main.o
