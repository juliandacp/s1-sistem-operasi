#!/bin/bash
wget -q https://busybox.net/downloads/busybox-1.36.1.tar.bz2
tar -xf busybox-1.36.1.tar.bz2
cd busybox-1.36.1

make defconfig

sed -i 's/^.*CONFIG_STATIC.*/CONFIG_STATIC=y/' .config
make -j$(nproc)
make install CONFIG_PREFIX=../rootfs
cd ..
rm -rf busybox-1.36.1 busybox-1.36.1.tar.bz2

cd rootfs
mkdir -p bin sbin etc proc sys usr/bin usr/sbin dev home/nous root etc/init.d

echo "root:x:0:0:root:/root:/bin/sh" > etc/passwd
echo "nous:x:1000:1000:Linux User,,,:/home/nous:/bin/sh" >> etc/passwd
echo "root:*::0:99999:7:::" > etc/shadow
echo "nous:\$1\$herta\$k9T8E3n0M4i8v.kP5Lg6Z.:19000:0:99999:7:::" >> etc/shadow
chmod 600 etc/shadow

echo "root:x:0:" > etc/group
echo "nous:x:1000:" >> etc/group
echo "Herta's Secret: Silver Wolf actually sucks at osu!" > home/nous/secret.txt
chown -R 1000:1000 home/nous

cat << 'EOF' > etc/inittab
::sysinit:/etc/init.d/rcS
tty1::respawn:/sbin/getty -L tty1 115200 vt100
::ctrlaltdel:/sbin/reboot
EOF

cat << 'EOF' > etc/init.d/rcS
#!/bin/sh
hostname HSS
EOF
chmod +x etc/init.d/rcS

cat << 'EOF' > init
#!/bin/sh
mount -t proc none /proc
mount -t sysfs none /sys
mount -t devtmpfs none /dev

if grep -q "single" /proc/cmdline; then
    echo "HERTA STATION: EMERGENCY ACCESS MODE ACTIVATED."
    exec /bin/sh
else
    exec /sbin/init
fi
EOF
chmod +x init

find . | cpio -o -H newc > ../rootfs.cpio
cd ..
gzip -f rootfs.cpio
rm -rf rootfs
