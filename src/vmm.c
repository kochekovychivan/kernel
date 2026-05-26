#include"defs.h"

static char* kernel_end; 
static u32* bootpg_dir;

typedef u32 pde;
typedef u32 pte;

pde* page_dir;

static inline void
cr3_reload(void)
{
  u32 cr3;

  __asm__ volatile("mov %%cr3, %0" : "=r"(cr3));
  __asm__ volatile("mov %0, %%cr3" : : "r"(cr3) : "memory");
}

void
vmm_init(void)
{
  kernel_end = PAGEROUNDUP((u32)&_kernel_end);
 
  page_dir = (pde*)boot_page_directory;

  page_dir[1023] = ((u32)page_dir) | PAGEPRESENT | PAGERW;

  cr3_reload();

  print_vga_str("VMM Initialized\n");
}

static pte* 
pt_get(u32 virt, int crt)
{
  u32 pd_idx = virt >> 22;
 
  if(!(page_dir[pd_idx] & PAGEPRESENT))
  {
    if(!crt) return NULL;

	u32 p = (u32)page_alloc();
	if(!p) panic("Failed to alloc a page for page table!\n");

	memset((void*)PTOV(p), 0, PAGESIZE);

	page_dir[pd_idx] = p | PAGEPRESENT | PAGERW;
	
	cr3_reload();
  }

  return (pte*)(0xFFC00000 + (pd_idx << 12));
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


