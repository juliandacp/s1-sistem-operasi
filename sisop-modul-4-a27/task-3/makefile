# =========================================================
# Task E — Complete every target below.
# All compiled outputs must go into the bin/ directory.
# =========================================================

prepare:
	# TODO: Create the bin/ directory if it does not exist.
	# TODO: Generate a blank 1.44 MB floppy image at bin/floppy.img.
	#       Use dd: input /dev/zero, 512-byte blocks, 2880 blocks total.
	mkdir -p bin
	dd if=/dev/zero of=bin/floppy.img bs=512 count=2880

bootloader:
	# TODO: Assemble src/bootloader.asm → bin/bootloader.bin  (nasm, flat binary format).
	# TODO: Write bin/bootloader.bin to sector 0 of bin/floppy.img
	#       (dd: 1 block, conv=notrunc so the rest of the image is preserved).
	nasm -f bin src/bootloader.asm -o bin/bootloader.bin
	dd if=bin/bootloader.bin of=bin/floppy.img bs=512 count=1 conv=notrunc

stdlib:
	bcc -ansi -c src/std_lib.c -o bin/std_lib.o -Iinclude
	# TODO: Compile src/std_lib.c → bin/std_lib.o  (bcc, -ansi -c flags).

kernel:
	bcc -ansi -c src/kernel.c -o bin/kernel.o -Iinclude
	nasm -f as86 src/kernel.asm -o bin/kernel_asm.o
	# TODO: Compile src/kernel.c   → bin/kernel.o     (bcc, -ansi -c flags).
	# TODO: Assemble src/kernel.asm → bin/kernel_asm.o (nasm, as86 format).

link:
	ld86 -o bin/kernel.bin -d bin/kernel.o bin/kernel_asm.o bin/std_lib.o
	dd if=bin/kernel.bin of=bin/floppy.img bs=512 seek=1 count=15 conv=notrunc

	# TODO: Link bin/kernel.o + bin/kernel_asm.o + bin/std_lib.o → bin/kernel.bin
	#       (ld86 with -d flag for no-standard-library linking).
	# TODO: Write bin/kernel.bin to bin/floppy.img starting at sector 1
	#       (dd: seek=1, conv=notrunc).

build: prepare bootloader stdlib kernel link

run:
	# TODO: Launch Bochs using bochsrc.txt in quiet mode (-q flag).
	bochs -f bochsrc.txt -q

