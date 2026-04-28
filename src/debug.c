#include"defs.h"

void
panic(const char* msg)
{
  __asm__ volatile("cli");

  print_vga_str("KERNEL PANIC: ");
  print_vga_str(msg);

  for(;;) __asm__ volatile("hlt");
}
