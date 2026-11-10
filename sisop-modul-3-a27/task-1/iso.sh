#!/bin/bash

mkdir -p mylinuxiso/boot/grub

cp osboot/bzImage mylinuxiso/boot/
cp osboot/single.gz mylinuxiso/boot/
cp osboot/multi.gz mylinuxiso/boot/

echo "set timeout=5" > mylinuxiso/boot/grub/grub.cfg
echo "set default=0" >> mylinuxiso/boot/grub/grub.cfg

echo "menuentry 'MyLinux: Single User' {" >> mylinuxiso/boot/grub/grub.cfg
echo "linux /boot/bzImage" >> mylinuxiso/boot/grub/grub.cfg
echo "initrd /boot/single.gz" >> mylinuxiso/boot/grub/grub.cfg
echo "}" >> mylinuxiso/boot/grub/grub.cfg

echo "menuentry 'MyLinux: Multi User' {" >> mylinuxiso/boot/grub/grub.cfg
echo "linux /boot/bzImage" >> mylinuxiso/boot/grub/grub.cfg
echo "initrd /boot/multi.gz" >> mylinuxiso/boot/grub/grub.cfg
echo "}" >> mylinuxiso/boot/grub/grub.cfg

grub-mkrescue -o osboot/mylinux.iso mylinuxiso

rm -rf mylinuxiso

echo "osboot/mylinux.iso"