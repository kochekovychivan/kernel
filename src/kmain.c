#include"defs.h"

void
kernel_main(u32 magic, struct multiboot_info* boot_info)
{
  clear_vga();
  print_vga_str("Kernel starting...\n");
  mem_init(boot_info);
  print_vga_str("PMM initialized\n");


  for(;;) __asm__ volatile("hlt");
}

