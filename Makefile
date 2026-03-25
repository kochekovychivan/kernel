
SRC_DIR = src
BUILD_DIR = build

all: $(BUILD_DIR)/kernel.bin

$(BUILD_DIR)/kernel.bin: $(BUILD_DIR)/kernel.o $(BUILD_DIR)/entry.o $(SRC_DIR)/linker.ld
	ld -m elf_i386 -T $(SRC_DIR)/linker.ld -nostdlib -o $@ $(BUILD_DIR)/kernel.o $(BUILD_DIR)/entry.o

$(BUILD_DIR)/kernel.o: $(SRC_DIR)/kernel.c
	gcc -m32 -ffreestanding -fno-stack-protector -c $< -o $@

$(BUILD_DIR)/entry.o: $(SRC_DIR)/entry.asm
	nasm -f elf32 $< -o $@

run: $(BUILD_DIR)/kernel.bin
	qemu-system-i386 -kernel $< -m 512M 
	
clean:
	rm -rf $(BUILD_DIR)/* $(KBIN_DIR)/kernel.bin kernel.iso
