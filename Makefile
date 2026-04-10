
SRC_DIR = src
BUILD_DIR = build
KO_DIR = src/my_kernel/boot
CONF_DIR = src/my_kernel

all: kernel.iso

kernel.iso: $(KO_DIR)/kernel
	grub-mkrescue -o kernel.iso $(CONF_DIR)

$(KO_DIR)/kernel: $(BUILD_DIR)/kmain.o $(BUILD_DIR)/entry.o $(BUILD_DIR)/vga.o $(BUILD_DIR)/string.o $(SRC_DIR)/linker.ld
	ld -m elf_i386 -T $(SRC_DIR)/linker.ld -nostdlib -o $@ $(BUILD_DIR)/entry.o $(BUILD_DIR)/kmain.o $(BUILD_DIR)/vga.o $(BUILD_DIR)/string.o

$(BUILD_DIR)/kmain.o: $(SRC_DIR)/kmain.c
	gcc -m32 -fno-stack-protector -fno-builtin -c $< -o $@

$(BUILD_DIR)/entry.o: $(SRC_DIR)/entry.asm
	nasm -f elf32 $< -o $@

$(BUILD_DIR)/vga.o: $(SRC_DIR)/vga.c
	gcc -m32 -fno-stack-protector -fno-builtin -c $< -o $@

$(BUILD_DIR)/string.o: $(SRC_DIR)/string.c
	gcc -m32 -fno-stack-protector -fno-builtin -c $< -o $@

run: kernel.iso
	qemu-system-x86_64 -cdrom kernel.iso

clean:
	rm -rf $(BUILD_DIR)/* 
	rm kernel.iso
	rm $(KO_DIR)/kernel
