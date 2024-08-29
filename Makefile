OBJECTS = build/kernel.asm.o build/kernel.o build/memory/heap/heap.o build/memory/heap/kheap.o build/memory/memory.o build/memory/paging/paging.o build/idt/idt.o build/memory/paging/paging.asm.o build/io/io.asm.o build/idt/idt.asm.o
FLAGS = -Ikernel
LINKER_FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc
.PHONY: all
all: bin/os.bin

.PHONY: clean
clean:
	@rm -rf bin
	@rm -rf build
	@echo

bin/os.bin: bin/boot.bin bin/kernel.bin
	@echo Building the system...
	@rm -rf bin/os.bin
	@dd if=bin/boot.bin >> bin/os.bin
	@dd if=bin/kernel.bin >> bin/os.bin
	@dd if=/dev/zero bs=512 count=100 >> bin/os.bin
	@echo Done!

bin/kernel.bin: $(OBJECTS)
	@echo Building the kernel...
	@$(TOOLCHAIN)-ld -g -relocatable $(OBJECTS) -o build/kernelfull.o
	@$(TOOLCHAIN)-gcc -T linker.ld -o bin/kernel.bin -ffreestanding -O0 -nostdlib build/kernelfull.o

bin/boot.bin: bootloader/$(ARCH)/boot.asm
	@echo Building the bootloader...
	@mkdir -p bin
	@nasm -f bin bootloader/$(ARCH)/boot.asm -o bin/boot.bin
build/kernel.o: kernel/kernel.c
	@echo Building kernel/kernel.c...
	@mkdir -p build
	@$(TOOLCHAIN)-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c kernel/kernel.c -o build/kernel.o

build/memory/heap/heap.o: kernel/memory/heap/heap.c
	@echo Building kernel/memory/heap/heap.c...
	@mkdir -p build/memory/heap
	@$(TOOLCHAIN)-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c kernel/memory/heap/heap.c -o build/memory/heap/heap.o

build/memory/heap/kheap.o: kernel/memory/heap/kheap.c
	@echo Building kernel/memory/heap/kheap.c...
	@mkdir -p build/memory/heap
	@$(TOOLCHAIN)-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c kernel/memory/heap/kheap.c -o build/memory/heap/kheap.o

build/memory/memory.o: kernel/memory/memory.c
	@echo Building kernel/memory/memory.c...
	@mkdir -p build/memory
	@$(TOOLCHAIN)-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c kernel/memory/memory.c -o build/memory/memory.o

build/memory/paging/paging.o: kernel/memory/paging/paging.c
	@echo Building kernel/memory/paging/paging.c...
	@mkdir -p build/memory/paging
	@$(TOOLCHAIN)-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c kernel/memory/paging/paging.c -o build/memory/paging/paging.o

build/idt/idt.o: kernel/idt/idt.c
	@echo Building kernel/idt/idt.c...
	@mkdir -p build/idt
	@$(TOOLCHAIN)-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c kernel/idt/idt.c -o build/idt/idt.o

build/kernel.asm.o: kernel/kernel.asm
	@echo Building kernel/kernel.asm...
	@mkdir -p build
	@nasm -f elf -g kernel/kernel.asm -o build/kernel.asm.o

build/memory/paging/paging.asm.o: kernel/memory/paging/paging.asm
	@echo Building kernel/memory/paging/paging.asm...
	@mkdir -p build/memory/paging
	@nasm -f elf -g kernel/memory/paging/paging.asm -o build/memory/paging/paging.asm.o

build/io/io.asm.o: kernel/io/io.asm
	@echo Building kernel/io/io.asm...
	@mkdir -p build/io
	@nasm -f elf -g kernel/io/io.asm -o build/io/io.asm.o

build/idt/idt.asm.o: kernel/idt/idt.asm
	@echo Building kernel/idt/idt.asm...
	@mkdir -p build/idt
	@nasm -f elf -g kernel/idt/idt.asm -o build/idt/idt.asm.o

