#include"defs.h"

void
kernel_main(u32 magic, struct multiboot_info* boot_info)
{
  clear_vga();
  mem_init(boot_info);

  while(1) {asm volatile("hlt");}
}

