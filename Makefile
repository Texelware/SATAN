OBJECTS = build/arch/x86/kernel.asm.o build/memory/paging/paging.asm.o build/arch/x86/io/io.asm.o build/arch/x86/idt/idt.asm.o build/kernel.o build/memory/heap/heap.o build/memory/heap/kheap.o build/memory/memory.o build/memory/paging/paging.o build/arch/x86/idt/idt.o
FLAGS = -Ikernel -I"kernel/arch/x86"
LINKER_FLAGS = -O0
.PHONY: all
all: bin/os.bin

.PHONY: clean
clean:
	@rm -rf bin
	@rm -rf build
	@echo

bin/os.bin: bin/boot.bin bin/kernel.bin
	@echo "[32mBuilding the system...(B[m"
	@rm -rf bin/os.bin
	@dd if=bin/boot.bin >> bin/os.bin
	@dd if=bin/kernel.bin >> bin/os.bin
	@dd if=/dev/zero bs=512 count=100 >> bin/os.bin
	@echo Done!

bin/kernel.bin: $(OBJECTS)
	@echo "[32mBuilding the kernel...(B[m"
	@$(TOOLCHAIN)-ld $(LINKER_FLAGS) -g -relocatable $(OBJECTS) -o build/kernelfull.o
	@$(TOOLCHAIN)-gcc $(LINKER_FLAGS) -T linker.ld -o bin/kernel.bin -ffreestanding -nostdlib build/kernelfull.o

bin/boot.bin: bootloader/x86/boot.asm
	@echo "[32mBuilding the bootloader...(B[m"
	@mkdir -p bin
	@nasm -f bin bootloader/x86/boot.asm -o bin/boot.bin
build/arch/x86/kernel.asm.o: kernel/arch/x86/kernel.asm
	@echo "[32m[0%](B[m Building kernel/arch/x86/kernel.asm..."
	@mkdir -p build/arch/x86
	@nasm -f elf -g kernel/arch/x86/kernel.asm -o build/arch/x86/kernel.asm.o

build/memory/paging/paging.asm.o: kernel/memory/paging/paging.asm
	@echo "[32m[10%](B[m Building kernel/memory/paging/paging.asm..."
	@mkdir -p build/memory/paging
	@nasm -f elf -g kernel/memory/paging/paging.asm -o build/memory/paging/paging.asm.o

build/arch/x86/io/io.asm.o: kernel/arch/x86/io/io.asm
	@echo "[32m[20%](B[m Building kernel/arch/x86/io/io.asm..."
	@mkdir -p build/arch/x86/io
	@nasm -f elf -g kernel/arch/x86/io/io.asm -o build/arch/x86/io/io.asm.o

build/arch/x86/idt/idt.asm.o: kernel/arch/x86/idt/idt.asm
	@echo "[32m[30%](B[m Building kernel/arch/x86/idt/idt.asm..."
	@mkdir -p build/arch/x86/idt
	@nasm -f elf -g kernel/arch/x86/idt/idt.asm -o build/arch/x86/idt/idt.asm.o

build/kernel.o: kernel/kernel.c
	@echo "[32m[40%](B[m Building kernel/kernel.c..."
	@mkdir -p build
	@$(TOOLCHAIN)-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c kernel/kernel.c -o build/kernel.o

build/memory/heap/heap.o: kernel/memory/heap/heap.c
	@echo "[32m[50%](B[m Building kernel/memory/heap/heap.c..."
	@mkdir -p build/memory/heap
	@$(TOOLCHAIN)-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c kernel/memory/heap/heap.c -o build/memory/heap/heap.o

build/memory/heap/kheap.o: kernel/memory/heap/kheap.c
	@echo "[32m[60%](B[m Building kernel/memory/heap/kheap.c..."
	@mkdir -p build/memory/heap
	@$(TOOLCHAIN)-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c kernel/memory/heap/kheap.c -o build/memory/heap/kheap.o

build/memory/memory.o: kernel/memory/memory.c
	@echo "[32m[70%](B[m Building kernel/memory/memory.c..."
	@mkdir -p build/memory
	@$(TOOLCHAIN)-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c kernel/memory/memory.c -o build/memory/memory.o

build/memory/paging/paging.o: kernel/memory/paging/paging.c
	@echo "[32m[80%](B[m Building kernel/memory/paging/paging.c..."
	@mkdir -p build/memory/paging
	@$(TOOLCHAIN)-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c kernel/memory/paging/paging.c -o build/memory/paging/paging.o

build/arch/x86/idt/idt.o: kernel/arch/x86/idt/idt.c
	@echo "[32m[90%](B[m Building kernel/arch/x86/idt/idt.c..."
	@mkdir -p build/arch/x86/idt
	@$(TOOLCHAIN)-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c kernel/arch/x86/idt/idt.c -o build/arch/x86/idt/idt.o

