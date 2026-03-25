
// Templates
void
print_vga(const char* msg, char* video);

// Main
void
kernel_main()
{
  char* video = (char*)0xB8000;
  const char* msg = "Hello, kernel!";
  print_vga(msg, video);
  while(1) {asm volatile("hlt");}
}

// Functions
void
print_vga(const char* msg, char* video)
{
  int row = 0;
  int col = 0;

  for(int i = 0; msg[i] != '\0'; i++)
  {
    if(msg[i] == '\n')
	{
	  row++;
	  col = 0;
	  continue;
	}

	int pos = (row * 80 + col) * 2;
    video[pos] = msg[i];
	video[pos+1] = 0x07;
	col++;

	if(col >= 80)
	{
	  row++;
	  col = 0;
	}
	if(row >= 25)
	{
	  row = 24;
	}
  }
}
