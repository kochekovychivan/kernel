#pragma once

//                          !! TYPES !!
#ifdef __x86_64__
typedef unsigned long size_t;
#else
typedef unsigned int size_t;
#endif

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

//--------------------------------------------------------\\

//                          !! MACRO !!
extern char _kernel_end[];
extern u32 boot_page_directory[];
extern u32 boot_page_table1[];

#define VGAADDR 0xC03FF000

#define PAGESIZE 4096
#define PDESIZE 1024
#define PTESIZE 1024
#define BITMAPARRSIZE 32768
#define TOTALPAGES 1048576
#define PAGEROUNDUP(sz)  (((sz)+PAGESIZE-1) & ~(PAGESIZE-1))
#define PAGEROUNDDOWN(a) (((a)) & ~(PAGESIZE-1))

#define KERNELSTART 0xC0000000
#define PTOV(x) ((void*)((u32)(x) + KERNELSTART))
#define VTOP(x) ((u32)(x) - KERNELSTART)

#define PAGEPRESENT 0x001
#define PAGERW 0x002
#define PAGEUSER 0x004

#define NULL 0

//--------------------------------------------------------\\

//                          !! STRUCTS !!
struct spinlock
{
  u32 locked;
};

struct multiboot_aout_symbol_table
{
  u32 tabsize;
  u32 strsize;
  u32 addr;
  u32 reserved;
};

struct multiboot_elf_section_header_table
{
  u32 num;
  u32 size;
  u32 addr;
  u32 shndx;
};

struct multiboot_info
{
  u32 flags;
  u32 mem_lower;
  u32 mem_upper;
  u32 boot_device;

  u32 cmdline;
  u32 mods_count;
  u32 mods_addr;

  union
  {
    struct multiboot_aout_symbol_table aout_sym;
    struct multiboot_elf_section_header_table elf_sec;
  }u;

  u32 mmap_length;
  u32 mmap_addr;

  u32 drives_length;
  u32 drives_addr;

  u32 config_table;
  u32 boot_loader_name;

  u32 apm_table;

  u32 vbe_control_info;
  u32 vbe_mode_info;
  u16 vbe_mode;
  u16 vbe_interface_seg;
  u16 vbe_interface_off;
  u16 vbe_interface_len;
};

struct multiboot_mmap_entry
{
  u32 size;
  u32 addr_low;
  u32 addr_high;
  u32 len_low;
  u32 len_high;
#define MULTIBOOT_MEMORY_AVAILABLE              1
#define MULTIBOOT_MEMORY_RESERVED               2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE       3
#define MULTIBOOT_MEMORY_NVS                    4
#define MULTIBOOT_MEMORY_BADRAM                 5
  u32 type;
} __attribute__((packed));

enum Color
{
  RED,
  BLACK
};

struct node
{
  void* start;
  u32 size; // free pages
  struct node* left;
  struct node* right;
  struct node* parent;
  enum Color color;

  struct node* next;
};


//--------------------------------------------------------\\

//                          !! TEMPLATES !!
void
print_vga_str(char* str);                      // vga.c

void
print_vga_char(u8 c, int pos);                 // vga.c

void
clear_vga(void);                               // vga.c

void*
memset(void* s, int c, size_t n);              // string.c

void*
memcpy(void* d, const void* s, size_t n);      // string.c

void
pmm_init(struct multiboot_info* boot_info);    // pmm.c

void
page_free(void* p);                            // pmm.c

void*
page_alloc(void);                              // pmm.c

void
spin_lock(struct spinlock *lk);                // spinlock.c

void
spin_unlock(struct spinlock *lk);              // spinlock.c

void
spin_init(struct spinlock *lk);                // spinlock.c

void
panic(const char* msg);                        // debug.c

void
vmm_init(void);                                // vmm.c

void
map(u32 virt, u32 phys, u32 flags);            // vmm.c

void
unmap(u32 virt);                               // vmm.c

void
identity_map(u32 start, u32 end, u32 flags);   // vmm.c


