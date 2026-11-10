#!/bin/bash

mkdir -p osboot

wget https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.1.1.tar.xz
tar -xvf linux-6.1.1.tar.xz
cd linux-6.1.1

cp ../.config .config
make olddefconfig

make -j$(nproc)

cp arch/x86/boot/bzImage ../osboot/

cd ..
rm -rf linux-6.1.1
rm -f linux-6.1.1.tar.xz

echo "osboot/bzImage"