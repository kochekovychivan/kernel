#include"defs.h"

void
kernel_main(u32 magic, struct multiboot_info* boot_info)
{
  clear_vga();
  print_vga_str("Kernel starting...\n");
  pmm_init((u32)boot_info);  
  vmm_init();

  for(;;) __asm__ volatile("hlt");
}

