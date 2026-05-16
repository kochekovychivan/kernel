#include"defs.h"

volatile u16* vga = (u16*)VGAADDR;

static int row = 0;
static int col = 0;

void 
print_vga_char(u8 c, int pos)
{
  vga[pos] = (0x0F << 8) | c;
}

void
print_vga_str(char* str)
{
  int i = 0;
  while(str[i])
  {
    if(str[i] == '\n')
	{
      row++;
	  col = 0;
	}
	else
	{
      int pos = row * 80 + col;
	  print_vga_char(str[i], pos);
	  col++;
	  if(col >= 80)
	  {
	    col = 0;
		row++;
	  }
	}
	i++;
  }
}

void
clear_vga(void)
{
  for(int i = 0; i<80*25; i++) 
  {
    vga[i] = (0x0F << 8) | ' '; 
  }
  row = 0;
  col = 0;
}
