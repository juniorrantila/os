#!/bin/bash

set -xe

CC="ccache clang"
CFLAGS="-Wno-excessive-regsave -m32 -mno-mmx -mno-sse -mno-sse2 -ffreestanding --target=i386-freestanding -std=c23 -Wall -Wextra -pedantic -Werror -Wno-gnu-zero-variadic-macro-arguments -Isrc -O2 -nostdlib -mstack-alignment=8"

mkdir -p build/i386
mkdir -p build/i386/core
mkdir -p build/i386/arch/i386
mkdir -p build/i386/kernel
mkdir -p build/i386/prekernel

$CC -c $CFLAGS src/core/arith64.c -o build/i386/core/arith64.o
$CC -c $CFLAGS src/core/kprintf.c -o build/i386/core/kprintf.o -Wno-unused-function

$CC -c $CFLAGS src/arch/i386/acpi.c      -o build/i386/arch/i386/acpi.o
$CC -c $CFLAGS src/arch/i386/interrupt.c -o build/i386/arch/i386/interrupt.o
$CC -c $CFLAGS src/arch/i386/pci.c       -o build/i386/arch/i386/pci.o
$CC -c $CFLAGS src/arch/i386/serial.c    -o build/i386/arch/i386/serial.o

$CC -c $CFLAGS src/kernel/kernel.c    -o build/i386/kernel/kernel.o
$CC -c $CFLAGS src/kernel/multiboot.c -o build/i386/kernel/multiboot.o

$CC -c $CFLAGS src/main.c -o build/i386/main.o

nasm -felf32 src/prekernel/multiboot_header.nasm -o build/i386/prekernel/multiboot_header.o
nasm -felf32 src/prekernel/start.nasm            -o build/i386/prekernel/start.o

ld.lld \
    -m elf_i386 \
    --nmagic \
    -T meta/linker.ld \
    -o kernel.i386 \
    build/i386/core/arith64.o \
    build/i386/core/kprintf.o \
    build/i386/arch/i386/acpi.o \
    build/i386/arch/i386/interrupt.o \
    build/i386/arch/i386/pci.o \
    build/i386/arch/i386/serial.o \
    build/i386/kernel/kernel.o \
    build/i386/kernel/multiboot.o \
    build/i386/prekernel/multiboot_header.o \
    build/i386/prekernel/start.o \
    build/i386/main.o
