#include "page.h"



unsigned page_hash( const struct hash_elem *p_, void *aux){
	const struct page *p = hash_entry(p_, struct page, hash_elem);
	//swap_map_init();
	return hash_bytes( &p->addr, sizeof p->addr );
}

bool page_less(const struct hash_elem *a_, const struct hash_elem *b_, void *aux ){
	const struct page *a = hash_entry(a_, struct page, hash_elem);
	const struct page *b = hash_entry(b_, struct page, hash_elem);

	return a->addr < b->addr;
}

struct page* page_lookup(struct hash *pages, void *addr){
	struct page p;
	struct hash_elem *e = NULL;

	p.addr = addr;
	e = hash_find(pages, &p.hash_elem);
	return e != NULL ? hash_entry(e, struct page, hash_elem) : NULL;

}
