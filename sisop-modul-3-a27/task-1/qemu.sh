#!/bin/bash

if [ "$1" == "--single" ]; 
then
    echo "Booting Single-User..."
    qemu-system-x86_64 \
      -smp 2 \
      -m 256 \
      -display curses \
      -vga std \
      -kernel osboot/bzImage \
      -initrd osboot/single.gz 

elif [ "$1" == "--multi" ];
then
    echo "Booting Multi-User..."
    qemu-system-x86_64 \
      -smp 2 \
      -m 256 \
      -display curses \
      -vga std \
      -kernel osboot/bzImage \
      -initrd osboot/multi.gz 

elif [ "$1" == "--all" ];
then
    echo "Booting ISO..."
    qemu-system-x86_64 \
      -smp 2 \
      -m 256 \
      -display curses \
      -vga std \
      -cdrom osboot/mylinux.iso
fi