#!/bin/bash

mkdir -p myramdisk/{bin,dev,proc,sys}

cp -a /dev/null myramdisk/dev
cp -a /dev/tty* myramdisk/dev
cp -a /dev/zero myramdisk/dev
cp -a /dev/console myramdisk/dev

cp /usr/bin/busybox myramdisk/bin
cd myramdisk/bin
./busybox --install .

cd ../..

echo "#!/bin/sh" > myramdisk/init
echo "/bin/mount -t proc none /proc" >> myramdisk/init
echo "/bin/mount -t sysfs none /sys" >> myramdisk/init
echo "exec /bin/sh" >> myramdisk/init

chmod +x myramdisk/init

cd myramdisk
find . | cpio -oH newc | gzip > ../osboot/single.gz
cd ..

rm -rf myramdisk

echo "osboot/single.gz"