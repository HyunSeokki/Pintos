#ifndef __FRAME_H__
#define __FRAME_H__

#include <hash.h>

struct frame{
	struct hash_elem hash_elem;
	void *addr; /* Vitual address */
};

void frame_init(void);

unsigned frame_hash( const struct hash_elem *f_, void *aux);
bool frame_less(const struct hash_elem *a_, const struct hash_elem *b_, void *aux);

struct frame* frame_lookup(struct hash *pages, void *addr);
#endif
