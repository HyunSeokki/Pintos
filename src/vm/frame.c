#include "frame.h"

#include <hash.h>


void frame_init(void){

	return ;
}



unsigned frame_hash( const struct hash_elem *f_, void *aux){
	const struct frame *f = hash_entry(f_, struct frame, hash_elem);
	//swap_map_init();
	return hash_bytes( &f->addr, sizeof f->addr );
}

bool frame_less(const struct hash_elem *a_, const struct hash_elem *b_, void *aux ){
	const struct frame *a = hash_entry(a_, struct frame, hash_elem);
	const struct frame *b = hash_entry(b_, struct frame, hash_elem);

	return a->addr < b->addr;
}

struct frame* frame_lookup(struct hash *pages, void *addr){
	struct frame f;
	struct hash_elem *e = NULL;

	f.addr = addr;
	e = hash_find(pages, &f.hash_elem);
	return e != NULL ? hash_entry(e, struct frame, hash_elem) : NULL;

}
