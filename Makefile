
SRC_DIR = src
BUILD_DIR = build
KO_DIR = src/iso/boot
CONF_DIR = src/iso

CC = i686-elf-gcc
LD = i686-elf-ld
AS = i686-elf-as

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

$(BUILD_DIR)/boot.o: $(SRC_DIR)/boot.s | $(BUILD_DIR)
	$(AS) --32 $< -o $@

$(BUILD_DIR)/debug.o: $(SRC_DIR)/debug.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/vmm.o: $(SRC_DIR)/vmm.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(KO_DIR)/kernel: $(BUILD_DIR)/kmain.o $(BUILD_DIR)/boot.o $(BUILD_DIR)/vga.o $(BUILD_DIR)/string.o $(BUILD_DIR)/spinlock.o $(BUILD_DIR)/pmm.o $(BUILD_DIR)/debug.o $(BUILD_DIR)/vmm.o
	$(LD) -T $(SRC_DIR)/linker.ld -nostdlib -o $@ $^

kernel.iso: $(KO_DIR)/kernel
	grub-mkrescue -o kernel.iso $(CONF_DIR)

run: kernel.iso
	qemu-system-i386 -cdrom kernel.iso

debug:
	qemu-system-i386 -cdrom kernel.iso -no-reboot -no-shutdown -d int,cpu_reset

clean:
	rm -rf $(BUILD_DIR)/* 
	rm kernel.iso
	rm $(KO_DIR)/kernel
