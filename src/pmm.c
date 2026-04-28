#include"defs.h"

static char* memory_start;

static u32 bitmap[BITMAPARRSIZE];

struct {
  struct spinlock lock;
  u32 use_lock;
  u32 free_pages;
} kmem;

void
mem_init(struct multiboot_info* boot_info)
{
  memset(bitmap, 0xFF, sizeof(bitmap));

  kmem.free_pages = 0;

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
          u32 idx = page >> 5; 
		  u32 bit = page & 31;

		  bitmap[idx] &= ~(1<<bit); 
          kmem.free_pages++;
		}
	  }
	}

	i += m->size + sizeof(m->size); 
  }

  kmem.use_lock = 1;
  spin_init(&kmem.lock);

  memory_start = (char*)PAGEROUNDUP((u32)end);

  for(u32 i = 0; i < (u32)memory_start; i += PAGESIZE)
  {
    u32 page = i >> 12;
    u32 idx = page >> 5;
	u32 bit = page & 31;

    bitmap[idx] |= (1 << bit);

	kmem.free_pages--;
  }

}

void
page_free(void* p)
{
  if((u32)p % PAGESIZE || (char*)p < memory_start || ((u32)p >> 12) >= TOTALPAGES) panic("Failed to free a page!\n"); 

  if(kmem.use_lock) spin_lock(&kmem.lock);

  u32 tmp = (u32)p >> 12;
  u32 idx = tmp >> 5;
  u32 bit = tmp & 31; 

  if(!((bitmap[idx] >> bit) & 1))
  {
    if(kmem.use_lock) spin_unlock(&kmem.lock);
    panic("double free");
  }
  bitmap[idx] &= ~(1<<bit);

  kmem.free_pages++;

  if(kmem.use_lock) spin_unlock(&kmem.lock);
}

void*
page_alloc(void)
{
  if(kmem.free_pages <= 0) return NULL;

  if(kmem.use_lock) spin_lock(&kmem.lock);

  for(u32 i = ((u32)memory_start >> 12); i<TOTALPAGES; i++)
  {
    u32 idx = i >> 5;
    u32 bit = i & 31;

    if(!((bitmap[idx] >> bit) & 1)) 
	{
      bitmap[idx] |= (1 << bit);

	  kmem.free_pages--;
	  
	  if(kmem.use_lock) spin_unlock(&kmem.lock);
	  
	  return (void*)(i << 12);
	}
  }

  if(kmem.use_lock) spin_unlock(&kmem.lock);
  return NULL;
}


