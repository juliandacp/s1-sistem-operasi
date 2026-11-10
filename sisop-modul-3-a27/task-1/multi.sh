#!/bin/bash

mkdir -p myramdisk/{bin,dev,proc,sys,etc,root,home/{blue,green,red,pink}}

cp -a /dev/null myramdisk/dev
cp -a /dev/tty* myramdisk/dev
cp -a /dev/zero myramdisk/dev
cp -a /dev/console myramdisk/dev

cp /usr/bin/busybox myramdisk/bin
cd myramdisk/bin
./busybox --install .
cd ../..

# Password User
root_pw=$(openssl passwd -1 root123)
blue_pw=$(openssl passwd -1 blue123)
green_pw=$(openssl passwd -1 green123)
red_pw=$(openssl passwd -1 red123)
pink_pw=$(openssl passwd -1 pink123)

# Konfigurasi User
echo "root:$root_pw:0:0:root:/root:/bin/sh" > myramdisk/etc/passwd
echo "blue:$blue_pw:1001:100:blue:/home/blue:/bin/sh" >> myramdisk/etc/passwd
echo "green:$green_pw:1002:100:green:/home/green:/bin/sh" >> myramdisk/etc/passwd
echo "red:$red_pw:1003:100:red:/home/red:/bin/sh" >> myramdisk/etc/passwd
echo "pink:$pink_pw:1004:100:pink:/home/pink:/bin/sh" >> myramdisk/etc/passwd

# Konfigurasi Group
echo "root:x:0:" > myramdisk/etc/group
echo "bin:x:1:root" >> myramdisk/etc/group
echo "sys:x:2:root" >> myramdisk/etc/group
echo "tty:x:5:root,blue,green,red,pink" >> myramdisk/etc/group
echo "disk:x:6:root" >> myramdisk/etc/group
echo "wheel:x:10:root,blue,green,red,pink" >> myramdisk/etc/group

echo "blue:x:1001:" >> myramdisk/etc/group
echo "green:x:1002:blue" >> myramdisk/etc/group
echo "red:x:1003:blue,green" >> myramdisk/etc/group
echo "pink:x:1004:blue,green,red" >> myramdisk/etc/group

# User Permission
chmod 700 myramdisk/root
chmod 750 myramdisk/home/blue
chmod 750 myramdisk/home/green
chmod 750 myramdisk/home/red
chmod 750 myramdisk/home/pink

chmod 755 myramdisk/bin
chmod 755 myramdisk/etc

# File init
echo "#!/bin/sh" > myramdisk/init
echo "/bin/mount -t proc none /proc" >> myramdisk/init
echo "/bin/mount -t sysfs none /sys" >> myramdisk/init
echo "echo 0 > /proc/sys/kernel/printk" >> myramdisk/init
echo "chown blue:blue /home/blue" >> myramdisk/init
echo "chown green:green /home/green" >> myramdisk/init
echo "chown red:red /home/red" >> myramdisk/init
echo "chown pink:pink /home/pink" >> myramdisk/init
echo "while true; do /bin/getty -L tty1 115200 vt100; done" >> myramdisk/init

chmod +x myramdisk/init

# Profile Welcome (ASCII Art Generator: ANSI Shadow)
echo "#!/bin/sh" > myramdisk/etc/profile
echo "clear" >> myramdisk/etc/profile
echo "echo '██╗  ██╗██╗'" >> myramdisk/etc/profile
echo "echo '██║  ██║██║'" >> myramdisk/etc/profile
echo "echo '███████║██║'" >> myramdisk/etc/profile
echo "echo '██╔══██║██║'" >> myramdisk/etc/profile
echo "echo '██║  ██║██║'" >> myramdisk/etc/profile
echo "echo '╚═╝  ╚═╝╚═╝'" >> myramdisk/etc/profile
echo "echo ''" >> myramdisk/etc/profile

echo "case \$(whoami) in" >> myramdisk/etc/profile
echo "root)" >> myramdisk/etc/profile
echo "echo '██████╗  ██████╗  ██████╗ ████████╗'" >> myramdisk/etc/profile
echo "echo '██╔══██╗██╔═══██╗██╔═══██╗╚══██╔══╝'" >> myramdisk/etc/profile
echo "echo '██████╔╝██║   ██║██║   ██║   ██║   '" >> myramdisk/etc/profile
echo "echo '██╔══██╗██║   ██║██║   ██║   ██║   '" >> myramdisk/etc/profile
echo "echo '██║  ██║╚██████╔╝╚██████╔╝   ██║   '" >> myramdisk/etc/profile
echo "echo '╚═╝  ╚═╝ ╚═════╝  ╚═════╝    ╚═╝   '" >> myramdisk/etc/profile
echo ";;" >> myramdisk/etc/profile

echo "blue)" >> myramdisk/etc/profile
echo "echo '██████╗ ██╗     ██╗   ██╗███████╗'" >> myramdisk/etc/profile
echo "echo '██╔══██╗██║     ██║   ██║██╔════╝'" >> myramdisk/etc/profile
echo "echo '██████╔╝██║     ██║   ██║█████╗  '" >> myramdisk/etc/profile
echo "echo '██╔══██╗██║     ██║   ██║██╔══╝  '" >> myramdisk/etc/profile
echo "echo '██████╔╝███████╗╚██████╔╝███████╗'" >> myramdisk/etc/profile
echo "echo '╚═════╝ ╚══════╝ ╚═════╝ ╚══════╝'" >> myramdisk/etc/profile
echo ";;" >> myramdisk/etc/profile

echo "green)" >> myramdisk/etc/profile
echo "echo ' ██████╗ ██████╗ ███████╗███████╗███╗   ██╗'" >> myramdisk/etc/profile
echo "echo '██╔════╝ ██╔══██╗██╔════╝██╔════╝████╗  ██║'" >> myramdisk/etc/profile
echo "echo '██║  ███╗██████╔╝█████╗  █████╗  ██╔██╗ ██║'" >> myramdisk/etc/profile
echo "echo '██║   ██║██╔══██╗██╔══╝  ██╔══╝  ██║╚██╗██║'" >> myramdisk/etc/profile
echo "echo '╚██████╔╝██║  ██║███████╗███████╗██║ ╚████║'" >> myramdisk/etc/profile
echo "echo ' ╚═════╝ ╚═╝  ╚═╝╚══════╝╚══════╝╚═╝  ╚═══╝'" >> myramdisk/etc/profile
echo ";;" >> myramdisk/etc/profile

echo "red)" >> myramdisk/etc/profile
echo "echo '██████╗ ███████╗██████╗ '" >> myramdisk/etc/profile
echo "echo '██╔══██╗██╔════╝██╔══██╗'" >> myramdisk/etc/profile
echo "echo '██████╔╝█████╗  ██║  ██║'" >> myramdisk/etc/profile
echo "echo '██╔══██╗██╔══╝  ██║  ██║'" >> myramdisk/etc/profile
echo "echo '██║  ██║███████╗██████╔╝'" >> myramdisk/etc/profile
echo "echo '╚═╝  ╚═╝╚══════╝╚═════╝ '" >> myramdisk/etc/profile
echo ";;" >> myramdisk/etc/profile

echo "pink)" >> myramdisk/etc/profile
echo "echo '██████╗ ██╗███╗   ██╗██╗  ██╗'" >> myramdisk/etc/profile
echo "echo '██╔══██╗██║████╗  ██║██║ ██╔╝'" >> myramdisk/etc/profile
echo "echo '██████╔╝██║██╔██╗ ██║█████╔╝ '" >> myramdisk/etc/profile
echo "echo '██╔═══╝ ██║██║╚██╗██║██╔═██╗ '" >> myramdisk/etc/profile
echo "echo '██║     ██║██║ ╚████║██║  ██╗'" >> myramdisk/etc/profile
echo "echo '╚═╝     ╚═╝╚═╝  ╚═══╝╚═╝  ╚═╝'" >> myramdisk/etc/profile
echo ";;" >> myramdisk/etc/profile
echo "esac" >> myramdisk/etc/profile

chmod +x myramdisk/etc/profile

cd myramdisk
find . | cpio -oHnewc | gzip > ../osboot/multi.gz

cd ..
rm -rf myramdisk

echo "osboot/multi.gz"