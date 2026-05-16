#include"defs.h"

static char* kernel_start;
static char* kernel_end; 

typedef u32 pde;
typedef u32 pte;

pde* page_dir;

static void
paging_enable(void)
{
  u32 cr0;
  __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));

  cr0 |= (1 << 31);
  __asm__ volatile("mov %0, %%cr0" :: "r"(cr0));
}

void
page_fault_handler(void)
{
  u32 fault_addr;
  __asm__ volatile("mov %%cr2, %0" : "=r"(fault_addr));

  panic("Page fault!\n");
}

void
vmm_init(void)
{
  kernel_start = (char*)PAGEROUNDDOWN(0x200000);
  kernel_end = (char*)PAGEROUNDUP((u32)&end);

  page_dir = page_alloc();
  if(!page_dir) panic("Failed to alloc a page for page directory!\n");
  
  memset(page_dir, 0, PAGESIZE);

  identity_map(0x0, 0x400000, 0x3);
  identity_map((u32)&stack_bottom, (u32)&stack_top, 0x3);
  identity_map(0xB8000, 0xB8FFF, 0x3);

  __asm__ volatile("mov %0, %%cr3" :: "r"(page_dir));

  paging_enable();

  print_vga_str("VMM Initialized\n");
}

static pte* 
pt_get(u32 virt, int crt)
{
  u32 pd_idx = virt >> 22;
  pde entry = page_dir[pd_idx];
 
  if(entry & 1) return (pte*)(entry & 0xFFFFF000);
  if(!crt) return NULL;
  
  pte* pt = (pte*)page_alloc();
  if(!pt) panic("Failed to alloc a page for page table!\n"); 

  memset(pt, 0, PAGESIZE);

  page_dir[pd_idx] = (((u32)pt) & 0xFFFFF000) | 0x3;

  return pt;
}

static inline void
tlb_invalidate(u32 virt)
{
  __asm__ volatile("invlpg (%0)" :: "r"(virt) : "memory");
}

void
map(u32 virt, u32 phys, u32 flags)
{
  pte* pt = pt_get(virt, 1);
  u32 pt_idx = (virt >> 12) & 0x3FF;

  pt[pt_idx] = (phys & 0xFFFFF000) | flags;

  tlb_invalidate(virt);
}

void
unmap(u32 virt)
{
  pte* pt = pt_get(virt, 0);
  if(!pt) return;

  u32 pt_idx = (virt >> 12) & 0x3FF;
  pt[pt_idx] = 0;
  
  tlb_invalidate(virt); 
}

void
identity_map(u32 start, u32 end, u32 flags)
{
  start = PAGEROUNDDOWN(start);
  end = PAGEROUNDUP(end);

  for(u32 addr = start; addr < end; addr += PAGESIZE) map(addr, addr, flags);
}


