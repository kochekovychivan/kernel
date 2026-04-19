#include"defs.h"

static char* memory_start = end;        

struct run 
{
  struct run* next;
};

struct 
{
  struct spinlock lock;
  int use_lock;
  struct run* freelist;
} kmem;

void 
kinit(void)
{
  kmem.freelist = 0;
  kmem.use_lock = 0;
  spin_init(&kmem.lock); 
}

void
mem_init(struct multiboot_info* boot_info)
{
  kinit();

  for(u32 i = 0; i < boot_info->mmap_length; )
  {
    struct multiboot_mmap_entry* mmmt = (struct multiboot_mmap_entry*)(boot_info->mmap_addr + i);

	if(mmmt->type == 1)
	{
      u32 addr = mmmt->addr_low;
	  u32 len = mmmt->len_low;

      u32 startm = PAGEROUNDUP(addr);
      u32 endm = PAGEROUNDDOWN(addr+len);

	  if(endm <= (u32)memory_start) 
	  {
        i += mmmt->size + sizeof(mmmt->size);
		continue;
	  }

      if(startm < (u32)memory_start) startm = (u32)memory_start;
	  if(startm < endm) range_free((void*)startm, (void*)endm);
	}
	i += mmmt->size + sizeof(mmmt->size);
  }

  kmem.use_lock = 1;
}

void
page_free(void* p)
{
  struct run* r;

  if((u32)p % PAGESIZE || p < (void*)memory_start) return;

  memset(p, 1, PAGESIZE);

  r = (struct run*)p;
  
  if(kmem.use_lock) spin_lock(&kmem.lock);

  r->next = kmem.freelist;
  kmem.freelist = r;

  if(kmem.use_lock) spin_unlock(&kmem.lock);
}

void 
range_free(void* pstart, void* pend)
{
  char* s = (char*)PAGEROUNDUP((u32)pstart);
  char* e = (char*)PAGEROUNDDOWN((u32)pend);

  while(s < e)
  {
    page_free(s);
	s += PAGESIZE;
  }
}


void*
page_alloc(void)
{
  struct run* r;

  if(kmem.use_lock) spin_lock(&kmem.lock);

  r = kmem.freelist;
  if(r) kmem.freelist = r->next;

  if(kmem.use_lock) spin_unlock(&kmem.lock);

  if(!r) return 0; 

  return (void*)r;
}
