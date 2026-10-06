#include <stdio.h>
#include "../headers/allocator.h"


int main(void){
    malloc_init();
	void *a = my_malloc(100);
	int *b = my_malloc(100);
	
	printf("a = %p\n", a);
	printf("b = %p\n", b);
}