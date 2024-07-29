all:
	mkdir -p ./bin 
	nasm -f bin ./bootloader/x86/boot.asm -o ./bin/boot.bin