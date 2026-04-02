#include"types.h"
#include"vga.h"
#include"string.h"

void
kernel_main(void)
{
  print_vga_str("Hello, kernel!\n");

  clear_vga();

  print_vga_str("Screen has been cleared!\n");
  while(1) {asm volatile("hlt");}
}

