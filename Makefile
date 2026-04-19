
SRC_DIR = src
BUILD_DIR = build
KO_DIR = src/my_kernel/boot
CONF_DIR = src/my_kernel

CC = i686-elf-gcc
LD = i686-elf-ld
AS = nasm

all: kernel.iso

CFLAGS = -ffreestanding -fno-stack-protector -fno-builtin -O2

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/kmain.o: $(SRC_DIR)/kmain.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/vga.o: $(SRC_DIR)/vga.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/string.o: $(SRC_DIR)/string.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/spinlock.o: $(SRC_DIR)/spinlock.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/pmm.o: $(SRC_DIR)/pmm.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/entry.o: $(SRC_DIR)/entry.asm | $(BUILD_DIR)
	$(AS) -f elf32 $< -o $@

$(KO_DIR)/kernel: $(BUILD_DIR)/kmain.o $(BUILD_DIR)/entry.o $(BUILD_DIR)/vga.o $(BUILD_DIR)/string.o $(BUILD_DIR)/spinlock.o $(BUILD_DIR)/pmm.o
	$(LD) -T $(SRC_DIR)/linker.ld -nostdlib -o $@ $^

kernel.iso: $(KO_DIR)/kernel
	grub-mkrescue -o kernel.iso $(CONF_DIR)

run: kernel.iso
	qemu-system-x86_64 -cdrom kernel.iso

clean:
	rm -rf $(BUILD_DIR)/* 
	rm kernel.iso
	rm $(KO_DIR)/kernel
