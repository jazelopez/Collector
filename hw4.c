#include "memlib.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "mm.h"
#include "hw4.h"

#define WSIZE 4
#define HDRP(bp) ((unsigned int*)(bp)-WSIZE)

struct node * root_table[ROOT_NR] = {};

//used for the sweep sections 
int in_use(unsigned int * hdrp) {
  return (*hdrp) & 0x1u;
}

// the three lowest bits are not part of the size
// 0x1 is already used for free blocks; we use 0x2
#define MARK_BIT (((unsigned int)0x2))

// marking related operations
int is_marked(unsigned int * hdrp) {
  return ((*hdrp) & MARK_BIT) >> 1; // return 1 or 0
}

//mark function for mark phase 
void mark(unsigned int * hdrp) {
  (*hdrp) |= MARK_BIT;
}

//unmark- i THINK after the sweep phase 
void unmark(unsigned int * hdrp) {
  (*hdrp) &= ~MARK_BIT;
}

// same to GET_SIZE in mm.c
unsigned int block_size(unsigned int * hdrp) {
  return (*hdrp) & (~7u);
}

//points to next head in heap 
void* next_hdrp(unsigned int * hdrp) {
  const unsigned int size = block_size(hdrp);
  if (size == 0) {
    fprintf(stderr,"%s Panic, chunk is of zero size.\n", __func__);
    exit(-1);
  }
  hdrp = (unsigned int *)(((char *)hdrp) + size);
  return block_size(hdrp) ? hdrp : NULL;
}

void heap_stat(const char * msg)
{
  void *hdrp = mm_first_hdr(); //pointer to first header in head 
  size_t nr_inuse = 0;
  size_t sz_inuse = 0;
  size_t nr_free = 0;
  size_t sz_free = 0;

  while (hdrp && block_size(hdrp)) {
    if (in_use(hdrp)) {
      nr_inuse++;
      sz_inuse += block_size(hdrp);
    } else {
      nr_free++;
      sz_free += block_size(hdrp);
    }
    if (is_marked(hdrp))
      printf("%s WARNING: found a mark\n", __func__);
    hdrp = next_hdrp(hdrp);
  }

  printf("%s: %s: heapsize %zu  inuse %zu %zu  free %zu %zu\n", __func__, msg,
      mem_heapsize(), nr_inuse, sz_inuse, nr_free, sz_free);
}

// ========
// add helper functions here if any


// garbage collection: Mark and Sweep
void gc() {
	//-------mark phase---------- 
	for(int i= 0; i < ROOT_NR; i++){
		struct node * nodePtr =  mm_malloc(sizeof(struct node)+i);  
		nodePtr = root_table[i];
		//traversing through the linked list to mark each node that is reached 
		while(nodePtr != NULL){
			if(is_marked(HDRP(nodePtr)))//avoiding cycles 
				break;

			mark(HDRP(nodePtr)); 
			nodePtr = nodePtr->next; 
		}//end while 
	}//end for loop for mark  


	//------sweep phase------
	/*
	void *hdrp = mm_first_hdr(); //pointer to first header in head

	//itertate through heap 
	while (hdrp && block_size(hdrp)){
		if(is_marked(hdrp)){

		}
	}//end while
       */	
}//end gc 
