
SRC_DIR = src
BUILD_DIR = build

all: $(BUILD_DIR)/kernel.bin

$(BUILD_DIR)/kernel.bin: $(BUILD_DIR)/kmain.o $(BUILD_DIR)/entry.o $(BUILD_DIR)/vga.o $(BUILD_DIR)/string.o $(SRC_DIR)/linker.ld
	ld -m elf_i386 -T $(SRC_DIR)/linker.ld -nostdlib -o $@ $(BUILD_DIR)/kmain.o $(BUILD_DIR)/entry.o $(BUILD_DIR)/vga.o $(BUILD_DIR)/string.o

$(BUILD_DIR)/kmain.o: $(SRC_DIR)/kmain.c
	gcc -m32 -ffreestanding -fno-stack-protector -c $< -o $@

$(BUILD_DIR)/entry.o: $(SRC_DIR)/entry.asm
	nasm -f elf32 $< -o $@

$(BUILD_DIR)/vga.o: $(SRC_DIR)/vga.c
	gcc -m32 -ffreestanding -fno-stack-protector -c $< -o $@

$(BUILD_DIR)/string.o: $(SRC_DIR)/string.c
	gcc -m32 -ffreestanding -fno-stack-protector -c $< -o $@

run: $(BUILD_DIR)/kernel.bin
	qemu-system-i386 -kernel $< -m 512M 
	
clean:
	rm -rf $(BUILD_DIR)/* 
