#!/bin/bash

set -xe

qemu-system-i386 \
    -m 1024 \
    -nographic \
    -kernel ./kernel \
    -virtfs local,path=meta/userspace,mount_tag=host0,security_model=passthrough,id=host0 \
    -drive file=fat:rw:./meta/userspace \
    -append "${*:1}"
