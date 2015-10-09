ASM = nasm
ASMFLAGS =

all: $(ASM_FILES)
	$(ASM) $(ASMFLAGS) boot.asm -o boot.bin
	
clean:
	rm boot.bin
