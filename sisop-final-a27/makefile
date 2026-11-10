prepare:
	dd if=/dev/zero of=bin/floppy.img bs=512 count=2880

bootloader:
	nasm -f bin src/bootloader.asm -o bin/bootloader.bin

	dd if=bin/bootloader.bin of=bin/floppy.img bs=512 count=1 conv=notrunc

kernel:
	nasm -f as86 src/kernel.asm -o bin/kernel_asm.o
	bcc -ansi -c src/std_lib.c -o bin/std_lib.o -Iinclude
	bcc -ansi -c src/filesystem.c -o bin/filesystem.o -Iinclude
	bcc -ansi -c src/shell.c -o bin/shell.o -Iinclude
	bcc -ansi -c src/kernel.c -o bin/kernel_c.o -Iinclude
	
	ld86 -o bin/kernel.bin -d bin/kernel_c.o bin/kernel_asm.o bin/std_lib.o bin/filesystem.o bin/shell.o

	dd if=bin/kernel.bin of=bin/floppy.img bs=512 seek=1 conv=notrunc

build: prepare bootloader kernel

run:
	qemu-system-i386 -fda bin/floppy.img

generate:
	python3 test/generate.py ${test}