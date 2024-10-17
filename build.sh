#!/usr/bin/env bash

#if this is not your path , kindly change it
export PREFIX="$HOME/opt/cross"
export PATH="$PREFIX/bin:$PATH"

export ARCH=x86
export TOOLCHAIN=i686-elf
export QEMU_SYSTEM=x86_64

# Colors
if command -v tput &> /dev/null; then
    ncolors=$(tput colors)
    if [ -n "$ncolors" ] && [ $ncolors -ge 8 ]; then
		bold="$(tput bold)"
        underline="$(tput smul)"
        standout="$(tput smso)"
        normal="$(tput sgr0)"
        black="$(tput setaf 0)"
        red="$(tput setaf 1)"
        green="$(tput setaf 2)"
        yellow="$(tput setaf 3)"
        blue="$(tput setaf 4)"
        magenta="$(tput setaf 5)"
        cyan="$(tput setaf 6)"
        white="$(tput setaf 7)"
	fi
fi

# Makefile
generateMakefile() {
	echo "${green}Regenerating Makefile...${normal}"
	asm_files=("kernel/arch/$ARCH/kernel.asm")
	c_files=()
	objects=("build/arch/$ARCH/kernel.asm.o")

	for file in $(find kernel -type f -name '*.asm' ! -name kernel.asm \( -path "kernel/arch/$ARCH/*" -o ! -path "kernel/arch/*" \)); do
		asm_files+=($file)
		objects+=(build${file#kernel}.o)
	done

	for file in $(find kernel -type f -name '*.c'); do
		c_files+=($file)
		object=${file#kernel}
		objects+=(build${object%.c}.o)
	done

	TAB="$(printf '\t')"
	cat > Makefile <<-EOF
	OBJECTS = ${objects[*]}
    INCLUDES = -Ikernel -I"kernel/arch/$ARCH" -Iinc
    # -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions 
	FLAGS = -O0 -g -ffreestanding -fno-builtin -Wall -Werror -nostdlib -nostartfiles -nodefaultlibs
	LINKER_FLAGS = -O0 -nostartfiles
	.PHONY: all
	all: bin/os.bin

	.PHONY: clean
	clean:
	${TAB}@rm -rf bin
	${TAB}@rm -rf build
	${TAB}@echo "${green}Cleaned!${normal}"

	# TODO: Hardcoded to GRUB
	bin/os.bin: bootloader/$ARCH/boot/grub/grub.cfg bin/kernel.bin
	${TAB}@echo "${green}Building the system...${normal}"
	${TAB}@rm -rf bin/iso bin/os.iso
	${TAB}@mkdir -p bin/iso
	${TAB}@cp -r -T bootloader/$ARCH bin/iso
	${TAB}@cp bin/kernel.bin bin/iso/boot/kernel.bin
	${TAB}@grub-mkrescue -o bin/os.iso bin/iso
	${TAB}@echo Done!

	bin/kernel.bin: \$(OBJECTS) kernel/arch/$ARCH/linker.ld
	${TAB}@echo "${green}Building the kernel...${normal}"
	${TAB}@mkdir -p bin
	${TAB}@\$(TOOLCHAIN)-gcc \$(LINKER_FLAGS) -n \$(OBJECTS) -T kernel/arch/$ARCH/linker.ld -o bin/kernel.bin
	EOF

	((total=${#asm_files[@]} + ${#c_files[@]}))
	progress=0
	for file in "${asm_files[@]}"; do
		object=(build${file#kernel}.o)
		cat >> Makefile <<-EOF
		$object: $file
		${TAB}@echo "${green}[$((progress * 100 / total))%]${normal} Building $file..."
		${TAB}@mkdir -p $(dirname "$object")
		${TAB}@nasm -f elf -g $file -o $object

		EOF
		((progress++))
	done

	for file in "${c_files[@]}"; do
		object=${file%.c}
		object=build${object#kernel}.o
		cat >> Makefile <<-EOF
		$object: $file
		${TAB}@echo "${green}[$((progress * 100 / total))%]${normal} Building $file..."
		${TAB}@mkdir -p $(dirname "$object")
		${TAB}@\$(TOOLCHAIN)-gcc \$(INCLUDES) \$(FLAGS) -std=gnu99 -c $file -o $object

		EOF
		((progress++))
	done

	echo "${green}Done!${normal}"
}

# Utils
build() {
	generateMakefile
	make
}

run() {
	qemu-system-$QEMU_SYSTEM -cdrom bin/os.iso
}

print() {
	echo "ARCH: $ARCH"
	echo "TOOLCHAIN: $TOOLCHAIN"
	echo "QEMU System: $QEMU_SYSTEM"
}

help() {
	cat <<-EOF
	SATAN Build system
	Usage: ./build.sh [--arch x86] [--toolchain i686-elf] [--quemu-system x86_64] [command]
	When ran without command, REPL mode will be entered
	Commands:
	generate/regenerate - (re)generate the Makefile
	build - (re)generate the Makefile and build the OS
	run - build and run the OS
	print - print current parameters
	clean - remove all build artifacts
	help - show this message
	quit/exit/q - exit REPL
	EOF
}

command() {
	case $1 in
		generate|regenerate) generateMakefile;;
		build) build;;
		run) build && run;;
		print) print;;
		clean)  make clean;;
		help)  help;;
		quit|exit|q)  exit 0;;
		*) echo -e "Unknown command $1.\nUse help to see the available options";;
	esac
}

repl() {
	cat <<-"EOF"
	 .oooooo..o       .o.       ooooooooooooo       .o.       ooooo      ooo
	d8P'    `Y8      .888.      8'   888   `8      .888.      `888b.     `8'
	Y88bo.          .8"888.          888          .8"888.      8 `88b.    8
	 `"Y8888o.     .8' `888.         888         .8' `888.     8   `88b.  8
	     `"Y88b   .88ooo8888.        888        .88ooo8888.    8     `88b.8
	oo     .d8P  .8'     `888.       888       .8'     `888.   8       `888
	8""88888P'  o88o     o8888o     o888o     o88o     o8888o o8o        `8
	
	Type help for list of availble commands
	EOF
 	while true; do
 		printf "> "
 		read -e line
 		if [[ $line == \:* ]]; then
 			bash -c ${line#\:}
 		else
	 		command $line
 		fi
 	done
}

# Main
POSITIONAL_ARGS=()

while [[ $# -gt 0 ]]; do
	case $1 in
		--arch)
			export ARCH="$2"
			shift
			shift
		;;
		--toolchain)
			export TOOLCHAIN="$2"
			shift
			shift
		;;
		--qemu-system)
			export QEMU_SYSTEM="$2"
			shift
			shift
		;;
		-h|--help)
			help
			exit 0
		;;
		-*|--*)
			echo "Unknown option $1"
			echo "Use help to see the available options"
			exit 1
		;;
    *)
      POSITIONAL_ARGS+=("$1")
      shift
      ;;
  esac
done

set -- "${POSITIONAL_ARGS[@]}"
if [ $# -gt 0 ]; then
	command "$@"
else
	repl
fi
