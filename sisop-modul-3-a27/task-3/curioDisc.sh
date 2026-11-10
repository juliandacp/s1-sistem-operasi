#!/bin/bash
mkdir -p iso/boot/grub
cp bzImage iso/boot/
cp rootfs.cpio.gz iso/boot/

cat << 'EOF' > iso/boot/grub/grub.cfg
set timeout=5
set default=1

menuentry "Emergency Shell" {
    linux /boot/bzImage console=tty1 single
    initrd /boot/rootfs.cpio.gz
}

menuentry "HSS Secure Login" {
    linux /boot/bzImage console=tty1
    initrd /boot/rootfs.cpio.gz
}
EOF

grub-mkrescue -o aha.iso iso
rm -rf iso
