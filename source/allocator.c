#include <stdio.h>
#include <string.h>
	#ifdef _WIN32
		#include <windows.h>
	#elif defined(__linux__) || defined(__APPLE__)
		#include <sys/mman.h>
	#endif
#include "../headers/allocator.h"
#define HEAP_SIZE 4096

void *os_alloc(size_t size){
	#ifdef _WIN32
	
		return VirtualAlloc(
		NULL ,
		size,
		MEM_RESERVE | MEM_COMMIT ,
		PAGE_READWRITE
		);
		
	#elif defined(__linux__) || defined(__APPLE__)

		void* p = mmap(
        NULL,
       	size,
       	PROT_READ | PROT_WRITE,
	   	MAP_PRIVATE | MAP_ANONYMOUS,
    	-1,
        0
    	);
    	if (p == MAP_FAILED){
    		printf("Unable to initialize the linux heap ");
    		return
    	}
		return p;
	#else 
		printf("Unknown OS has been detected");
	#endif

}
void my_free(void *ptr);
void* my_malloc(size_t size);
void* my_calloc(size_t size);
void* my_realloc(void* ptr , size_t size);
void malloc_init();
void malloc_destroy(void);
typedef struct Block
{
	size_t size;
	int free;
	struct Block* next;

} Block;


unsigned char *heap;


Block *free_list = NULL;

void malloc_init(){
	heap = os_alloc(HEAP_SIZE);
	if (heap == NULL){
		printf("Failed to initialize heap memory");
		return ;
	}
	free_list = (Block *)heap;
	free_list->size = HEAP_SIZE - sizeof(Block);
	free_list->next = NULL;
	free_list->free = 1;

} 
void malloc_destroy(void)
{ 
	#ifdef _WIN32
    if (heap != NULL) {
        VirtualFree(heap, 0, MEM_RELEASE);
        heap = NULL;
        free_list = NULL;
    }
    #elif defined(__linux__) || defined(__APPLE__)
    if (heap != NULL){
    	munmap(heap, HEAP_SIZE );
    }
    #endif
    return;
}


void *my_malloc(size_t size){
	Block *current = free_list;
	while(current){
		if (current->size >= size && current->free ){
			current->free = 0;
			if (current->size - size >= sizeof(Block) &&
			current->size - size - sizeof(Block)> 0)
			{
			Block *new_block = (Block *)((unsigned char *)(current + 1) + size );
			new_block->size = current->size - size - sizeof(Block);
			new_block->free = 1;
			new_block->next = current->next;
			current->size = size;
			current->next = new_block;
		}
			return (void *)(current + 1);
		}
		current = current->next;

	}
	return NULL;

}

void *my_calloc(size_t size){
	Block *current = free_list;
	while(current){
		if (current->size >=size && current->free){
			current->free = 0;		
			unsigned char* User_space = (unsigned char *)(current + 1);
			for(int i = 0 ; i < (int)size ; ++i){
				User_space[i] = 0;
			}
			
			
			if (current->size - size >= sizeof(Block) && current->size - size - sizeof(Block)>0)
			{
			Block *new_block = (void *)((unsigned char *)(User_space) + size );
			new_block->size = current->size - size - sizeof(Block);
			new_block->free = 1; 
			new_block->next = current->next;
			current->size = size;
			current->next = new_block;
			}
			return (void *)(User_space);
		}
		current = current->next;
	}

	return NULL;
}

void *my_realloc(void* ptr , size_t size){
	if (ptr == NULL){
		return my_malloc(size);
	}
	Block *block = (Block *)ptr - 1;
	if (block->size >= size)
	{
		
		Block* new_block = (Block *)((unsigned char *)(block + 1 )+ size)  ;

		if (block->size - size >= sizeof(Block) &&
		 block->size - size - sizeof(Block) > 0)
		{
		new_block->size = block->size - size - sizeof(Block);
		block->size = size ;
		new_block->free = 1;
		Block* next = block->next;
		new_block->next = next;
		block->next = new_block;
		}
		return (void *)(block + 1);
	}else if (block->next != NULL && block->next->free == 1 &&
	 		 (block->next->size >= size - block->size))
	{

		Block *next = block->next->next;
		block->size += sizeof(Block) + block->next->size;
		block->next = next;
		if (block->size - size >= sizeof(Block) &&
		 block->size - size - sizeof(Block)> 0)
		{
			Block *new_block = (Block *)((unsigned char *)(block + 1 )+ size);
			new_block->free = 1;
			new_block->size = block->size - size - sizeof(Block);
			new_block->next = block->next;
			block->next = new_block;
		}
		return (void *)(block + 1);
	}
	else
	{
		void *p = my_malloc(size);
		memcpy(p , ptr , block->size);
		my_free(ptr);
		return p;
	}


}
void my_free(void *ptr){
	if (ptr == NULL)
		return ;
	Block *block = (Block *)ptr - 1;
	block->free = 1;
	if (block->next != NULL && block->next->free == 1 &&
		(unsigned char *)(block + 1) + 
		block->size == (unsigned char*)block->next)
	{

		block->size += sizeof(Block) + block->next->size;
		block->next = block->next->next;
	}
	return;

}


int my_write(void *p  , size_t offset, unsigned char data){
	Block* block = (Block *)p - 1;
	if (block->size <= offset ){
		return -1 ;
	}
	unsigned char *data_ptr = p;
	data_ptr[offset] = data;
	return 0;
}