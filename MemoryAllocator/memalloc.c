#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

typedef char ALIGN[16];

union header {
    struct header_t{
        size_t size;
        unsigned is_free;

        struct header_t *next;
    } s;
    ALIGN stub;
};
typedef union header header_t;
header_t *head = NULL, *tail = NULL;
pthread_mutex_t global_malloc_lock;

void free(void *block){

}

void *calloc(size_t num, size_t nsize){

}

void *realloc(void *block, size_t size){

}


void *malloc(size_t size){
    /**
 * void *malloc(size_t size) -> void *
 *
 * Allocates a block of memory of at least `size` bytes from the heap.
 *
 * This function is a custom implementation of the standard `malloc`. 
 * It first tries to find a free memory block of sufficient size from a 
 * linked list of previously allocated blocks. If a suitable free block 
 * is found, it is marked as in use and returned. Otherwise, it requests 
 * a new block of memory from the system using `sbrk`, creates a header 
 * to track the allocation, and adds it to the linked list of allocations.
 *
 * The returned pointer points to the memory region **immediately after 
 * the header**, so it can be safely used for storing user data.
 *
 * Thread safety: This implementation uses a global mutex (`global_malloc_lock`) 
 * to protect allocation operations in multithreaded programs.
 *
 * Preconditions:
 * - `size` must be greater than 0. If `size` is 0, the function returns NULL.
 * - The caller must eventually free the memory using a compatible `free` 
 *   implementation to avoid memory leaks.
 *
 * Returns:
 * - A pointer to a memory block of at least `size` bytes, or NULL if the 
 *   allocation fails (e.g., insufficient heap memory).
 */

    size_t total_size;
    void *block;
    header_t *header;
    if (!size){
        return NULL;
    }
    pthread_mutex_lock(&global_malloc_lock);
	header = get_free_block(size);

    if (header) {
		header->s.is_free = 0;
		pthread_mutex_unlock(&global_malloc_lock);
		return (void*)(header + 1);
	}
    
	total_size = sizeof(header_t) + size;
    block = sbrk(total_size);
    if (block == (void*) - 1){
        pthread_mutex_unlock(&global_malloc_lock);
        return NULL;
    }

    header = block;
	header->s.size = size;
	header->s.is_free = 0;
	header->s.next = NULL;
	if (!head)
		head = header;
	if (tail)
		tail->s.next = header;
	tail = header;
	pthread_mutex_unlock(&global_malloc_lock);
	return (void*)(header + 1);

    return block;
}

header_t *get_free_block(size_t size)
{
	header_t *curr = head;
	while(curr) {
		if (curr->s.is_free && curr->s.size >= size)
			return curr;
		curr = curr->s.next;
	}
	return NULL;
}