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
typedef unsigned long u64;

//--------------------------------------------------------\\

//                          !! MACRO !!
extern char end[];

#define VIRTMEMSTART   0xB8000

#define VTOP(a) (((u32)(a)) - VIRTMEMSTART) 
#define PTOV(a) ((void*)(((char*)(a)) + VIRTMEMSTART))

#define PAGESIZE 4096
#define PAGEROUNDUP(sz)  (((sz)+PAGESIZE-1) & ~(PAGESIZE-1))
#define PAGEROUNDDOWN(a) (((a)) & ~(PAGESIZE-1))

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


//--------------------------------------------------------\\

//                          !! TEMPLATES !!
void
print_vga_str(char* str);                      // vga.c

void
print_vga_char(u8 c, int pos);                 // vga.c

void
clear_vga(void);                               // vga.c

void*
memset(const void* s, int c, size_t n);        // string.c

void*
memcpy(void* d, const void* s, size_t n);      // string.c

void
kinit(void);                                   // pmm.c

void
mem_init(struct multiboot_info* boot_info);    // pmm.c

void
page_free(void* p);                            // pmm.c

void
range_free(void* pstart, void* pend);          // pmm.c

void*
page_alloc(void);                              // pmm.c

static inline u32
xchg(volatile u32* addr, u32 new_val);         // spinlock.c

void
spin_lock(struct spinlock *lk);                // spinlock.c

void
spin_unlock(struct spinlock *lk);              // spinlock.c

void
spin_init(struct spinlock *lk);                // spinlock.c










