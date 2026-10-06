#include <stdio.h>
#include "../headers/allocator.h"


int main(void){
    malloc_init();
	void *a = my_malloc(100);

	void *c = my_realloc(a , 100);
	printf("a = %p\n", a);
	printf("c = %p\n", c);
}