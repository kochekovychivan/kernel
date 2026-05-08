#include"defs.h"

#define NODESPERPAGE (PAGESIZE / sizeof(struct node))

void* ptr;
struct node* vmmroot;

static struct node* freelist;

static void
pool_init(void)
{
  void* page = ptr;

  ptr = (char*)ptr + PAGESIZE;

  struct node* nodes = (struct node*)page;

  for(u32 i = 0; i<NODESPERPAGE-1; i++)
  {
    nodes[i].next = &nodes[i+1];
  }

  nodes[NODESPERPAGE-1].next = NULL;
  freelist = &nodes[0];
}

static void
pool_grow(void)
{
  void* page = ptr;
   
  ptr = (char*)ptr + PAGESIZE;
  
  struct node* nodes = (struct node*)page;

  for(u32 i = 0; i<NODESPERPAGE-1; i++)
  {
    nodes[i].next = &nodes[i+1];
  }

  nodes[NODESPERPAGE-1].next = freelist;
  freelist = &nodes[0];
}

static struct node*
node_alloc(void)
{
  if(!freelist) pool_grow();

  struct node* n = freelist;
  freelist = freelist->next;

  memset(n, 0, sizeof(*n));
  return n; 
}

static void
node_free(struct node* n)
{
  n->next = freelist;
  freelist = n;
}

static struct node* 
node_create(void* start, u32 size) 
{
  struct node* n = node_alloc();
  n->start = start;
  n->size = size;
  n->color = RED;
  n->left = NULL;
  n->right = NULL;
  n->parent = NULL;
  return n;
}

static struct node* 
grandparent(struct node* n) 
{
  if(n && n->parent) return n->parent->parent;
  return NULL;
}

static struct node* 
uncle(struct node* n) 
{
  struct node* g = grandparent(n);
  if(!g) return NULL;
  if(n->parent == g->left) return g->right;
  else return g->left;
}

void 
rotate_left(struct node** root, struct node* n) 
{
  struct node* y = n->right;
  n->right = y->left;
  if(y->left) y->left->parent = n;
  y->parent = n->parent;
  if(!n->parent) *root = y;
  else if(n == n->parent->left) n->parent->left = y;
  else n->parent->right = y;
  y->left = n;
  n->parent = y;
}

void
rotate_right(struct node** root, struct node* n) 
{
  struct node* y = n->left;
  n->left = y->right;
  if(y->right) y->right->parent = n;
  y->parent = n->parent;
  if(!n->parent) *root = y;
  else if(n == n->parent->right) n->parent->right = y;
  else n->parent->left = y;
  y->right = n;
  n->parent = y;
}

static void 
insert_fix(struct node** root, struct node* n) 
{
  while(n != *root && n->parent && n->parent->color == RED)
  {
    struct node* g = grandparent(n);

    if(!g) break;

    struct node* u = uncle(n);

    if(u && u->color == RED)
    {
      n->parent->color = BLACK;
      u->color = BLACK;
      g->color = RED;
      n = g;
    }
    else
    {
      if(n->parent == g->left)
      {
        if(n == n->parent->right)
        {
          n = n->parent;
          rotate_left(root, n);
        }

        n->parent->color = BLACK;
        g->color = RED;
        rotate_right(root, g);
      }
      else
      {
        if(n == n->parent->left)
        {
          n = n->parent;
          rotate_right(root, n);
        }

        n->parent->color = BLACK;
        g->color = RED;
        rotate_left(root, g);
      }
    }
  }

  (*root)->color = BLACK;
}

static struct node*
insert(struct node* root, void* start, u32 size)
{
  struct node* n = node_create(start, size);
  if(!root)
  {
    n->color = BLACK;
    return n;
  }
  struct node* cur = root;
  struct node* parent = NULL;

  while(cur)
  {
    parent = cur;
    if(start < cur->start) cur = cur->left;
    else cur = cur->right;
  }
  n->parent = parent;

  if(start < parent->start) parent->left = n;
  else parent->right = n;

  insert_fix(&root, n);

  while(root->parent) root = root->parent;
  return root;
}

struct node*
vmm_init(struct multiboot_info* boot_info)
{
  ptr = pmm_init(boot_info);
  pool_init();  

  if(!ptr) panic("pmm_init returned NULL!\n");

  vmmroot = NULL;
   
  for(u32 i = 0; i<TOTALPAGES; i++)
  {
    if(page_check(i))
    {
      void* start = (void*)(i << 12);
      u32 size = region_size(i);
      vmmroot = insert(vmmroot, start, size);
      i+=size-1;
    }
  }
  
  print_vga_str("VMM Initialized\n");
 
  return vmmroot;
}
