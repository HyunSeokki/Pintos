#ifndef __VM_PAGE_H__
#define __VM_PAGE_H__

#include <hash.h>

struct page{
	/* Manual p88 */
	struct hash_elem hash_elem; /*Hash table element. */
	void *addr; /* Vitual address */
};

//unsigned 
unsigned page_hash( const struct hash_elem *p_, void *aux);
bool page_less(const struct hash_elem *a_, const struct hash_elem *b_, void *aux);

struct page* page_lookup(struct hash *pages, void *addr);



#endif
