#include"defs.h"

static char* kernel_start;
static char* kernel_end; 

static u32 bitmap[BITMAPARRSIZE];

struct {
  struct spinlock lock;
  u32 use_lock; 
} kmem;

static void 
reserve_region(u32 start, u32 size) 
{
  u32 s = PAGEROUNDDOWN(start);
  u32 e = PAGEROUNDUP(start + size);
  for(u32 p = s; p < e; p += PAGESIZE) 
  { 
    u32 page = p >> 12;
    if(page >= TOTALPAGES) panic("Failed to reserve a region!\n"); 
    bitmap[page >> 5] |= (1 << (page & 31)); 
  } 
}
       
void
pmm_init(struct multiboot_info* boot_info)
{
  kernel_start = (char*)PAGEROUNDDOWN(0x200000);
  kernel_end = (char*)PAGEROUNDUP((u32)end);
  memset(bitmap, 0xFF, sizeof(bitmap));

  if(!(boot_info->flags & (1 << 6))) panic("No memory map from multiboot");

  for(u32 i = 0; i<boot_info->mmap_length; )
  {
	struct multiboot_mmap_entry* m = (struct multiboot_mmap_entry*)(boot_info->mmap_addr + i);

	if(m->size == 0) panic("mmap size 0");
   
    if(m->type == MULTIBOOT_MEMORY_AVAILABLE) 
	{
	  u32 startp = PAGEROUNDUP(m->addr_low);
	  u32 endp = PAGEROUNDDOWN((m->addr_low + m->len_low));
	  u32 lim = (endp-startp) >> 12;

	  for(u32 j = 0; j<lim; j++)
	  {
	    u32 page = (startp+(j<<12)) >> 12;

		if(page < TOTALPAGES)
		{
		  bitmap[page >> 5] &= ~(1<<(page & 31)); 
		}
	  }
	}

	i += m->size + sizeof(m->size); 
  }

  reserve_region((u32)kernel_start, (u32)kernel_end - (u32)kernel_start);
  reserve_region((u32)boot_info, sizeof(struct multiboot_info));
  reserve_region(boot_info->mmap_addr, boot_info->mmap_length);
 
  bitmap[0 >> 5] |= (1 << (0 & 31));

  kmem.use_lock = 1;
  spin_init(&kmem.lock); 
  print_vga_str("PMM Initialized\n"); 
}

void
page_free(void* p)
{
  if((u32)p % PAGESIZE || ((u32)p >> 12) >= TOTALPAGES) panic("Failed to free a page!\n"); 

  if(kmem.use_lock) spin_lock(&kmem.lock);

  memset(p, 0xCC, PAGESIZE);

  u32 page = (u32)p >> 12;

  if(!((bitmap[page >> 5] >> (page & 31)) & 1))
  {
    if(kmem.use_lock) spin_unlock(&kmem.lock);
    panic("double free");
  }
  bitmap[page >> 5] &= ~(1<<(page & 31));

  if(kmem.use_lock) spin_unlock(&kmem.lock);
}

void*
page_alloc(void)
{
  if(kmem.use_lock) spin_lock(&kmem.lock);

  for(u32 i = 0; i<TOTALPAGES; i++)
  {
    if(!(bitmap[i >> 5] & (1 << (i & 31)))) 
	{
      bitmap[i >> 5] |= (1 << (i & 31));

	  if(kmem.use_lock) spin_unlock(&kmem.lock);
	  
	  return (void*)(i << 12);
	}
  }

  if(kmem.use_lock) spin_unlock(&kmem.lock);
  return NULL;
}

