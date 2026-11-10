#!/bin/bash
wget --show-progress https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.1.1.tar.xz

tar -xf linux-6.1.1.tar.xz
cd linux-6.1.1

make x86_64_defconfig

make -j$(nproc) bzImage

cd ..
cp arch/x86/boot/bzImage ../

rm -rf linux-6.1.1 linux-6.1.1.tar.xz
