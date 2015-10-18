ASM = nasm
ASMFLAGS =
GCC = $(HOME)/opt/cross/bin/i586-elf-gcc
GCCFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra

all: image kernel kernel-install

kernel:
	$(ASM) boot.asm -o boot.o -f elf
	$(GCC) -c kernel.c -o kernel.o $(GCCFLAGS)
	$(GCC) -T kernel.ld -o sos-kernel.img -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc

kernel-install:
	sudo losetup /dev/loop1 sos.img -o 1048576
	sudo mount /dev/loop1 /mnt
	sudo cp sos-kernel.img /mnt/boot/
	sudo umount /mnt
	sudo losetup -d /dev/loop1
	
image:
	dd if=/dev/zero of=sos.img bs=512 count=131072
	sfdisk sos.img < sos.img.layout
	
	sudo losetup /dev/loop0 sos.img
	sudo losetup /dev/loop1 sos.img -o 1048576
	sudo mke2fs /dev/loop1
	sudo mount /dev/loop1 /mnt
	sudo grub-install --root-directory=/mnt --no-floppy --modules="normal ext2 part_msdos multiboot" /dev/loop0
	sudo cp sos-kernel.img /mnt/boot/
	sudo cp grub/* /mnt/boot/grub/
	sudo losetup -d /dev/loop0
	sudo umount /mnt
	sudo losetup -d /dev/loop1
	
clean:
	rm *.o
	rm sos-kernel.img
	rm sos.img
