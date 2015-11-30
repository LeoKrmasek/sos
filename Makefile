ASM = nasm
ASMFLAGS =
GCC = $(HOME)/opt/cross/bin/i586-elf-gcc
GCCFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Iinclude
GCC_TESTING = gcc # Use native compiler for developer's platform
GCCFLAGS_TESTING = -m32 -std=gnu99 -O2 -Itest/include/libtap -Iinclude

C_OBJECTS = $(patsubst %.c, %.o, $(shell find . -path ./test -prune -o -name "*.c" -print))
ASM_OBJECTS = $(patsubst %.asm, %_asm.o, $(shell find . -name "*.asm"))
TEST_OBJECTS = $(patsubst %.c, %_test.o, $(shell find ./test/ -maxdepth 1 -name "*.c"))

all: image kernel kernel-install

%_test.o: %.c
	$(GCC_TESTING) -c $< -o $@ $(GCCFLAGS_TESTING)

%_asm.o: %.asm
	$(ASM) $< -o $@ -f elf

%.o: %.c
	$(GCC) -c $< -o $@ $(GCCFLAGS)

kernel: $(C_OBJECTS) $(ASM_OBJECTS)
	$(GCC) -T kernel.ld -o sos-kernel.img -ffreestanding -O2 -nostdlib $(C_OBJECTS) $(ASM_OBJECTS) -lgcc

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
	sudo cp grub/* /mnt/boot/grub/
	sudo losetup -d /dev/loop0
	sudo umount /mnt
	sudo losetup -d /dev/loop1

test: $(TEST_OBJECTS) $(C_OBJECTS) $(ASM_OBJECTS)
	$(GCC_TESTING) -c test/include/libtap/tap.c -o test/include/libtap/tap.o $(GCCFLAGS_TESTING)
	for file in $(TEST_OBJECTS); do \
		$(GCC_TESTING) $$file $(C_OBJECTS) $(ASM_OBJECTS) test/include/libtap/tap.o -o a.out -m32 ; \
		./a.out ; \
	done
	rm a.out

clean:
	rm $(ASM_OBJECTS) $(C_OBJECTS) $(TEST_OBJECTS)
	rm test/include/libtap/tap.o
	rm sos-kernel.img
	rm sos.img
