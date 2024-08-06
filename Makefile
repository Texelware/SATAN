INCLUDES = -I./kernel
FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

.PHONY: all
all: ./bin/os.bin

./bin/os.bin: ./bin/boot.bin ./bin/kernel.bin
	rm -rf ./bin/os.bin
	dd if=./bin/boot.bin >> ./bin/os.bin
	dd if=./bin/kernel.bin >> ./bin/os.bin
	dd if=/dev/zero bs=512 count=100 >> ./bin/os.bin

# Bootloader
./bin/boot.bin: ./bootloader/$(ARCH)/boot.asm
	mkdir -p bin
	nasm -f bin ./bootloader/$(ARCH)/boot.asm -o ./bin/boot.bin

# Kernel
# Note: Order of those matter V
KERNEL_SOURCES = ./kernel/arch/$(ARCH)/kernel.asm ./kernel/kernel.c
OBJS = $(patsubst ./kernel/%.c,./build/%.o,$(patsubst ./kernel/%.asm,./build/%.o,$(KERNEL_SOURCES)))

./build/%.o: ./kernel/%.c
	mkdir -p $(dir $@)
	$(TARGET)-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c $< -o $@

./build/%.o: ./kernel/%.asm
	mkdir -p $(dir $@)
	nasm -f elf -g $< -o $@

./bin/kernel.bin: $(OBJS)
	$(TARGET)-ld -g -relocatable $^ -o ./build/kernelfull.o
	$(TARGET)-gcc $(FLAGS) -T ./linker.ld -o ./bin/kernel.bin -ffreestanding -O0 -nostdlib ./build/kernelfull.o

clean:
	rm -rf ./bin/boot.bin
	rm -rf ./bin/kernel.bin
