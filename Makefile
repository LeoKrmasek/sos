ASM = nasm
ASMFLAGS =

all: $(ASM_FILES)
	$(ASM) $(ASMFLAGS) boot.asm -o boot.bin
	$(ASM) $(ASMFLAGS) kernel.asm -o kernel.bin
	
	dd if=/dev/zero of=sos.img bs=1k count=1440
	cat boot.bin kernel.bin > /tmp/sos.bin
	dd status=noxfer conv=notrunc if=/tmp/sos.bin of=sos.img
	
clean:
	rm boot.bin
	rm kernel.bin
	rm sos.img
