#include <stdio.h>
#include "../headers/allocator.h"


int main(void){
    malloc_init();
	void *a = my_malloc(100);

	my_free(a);

	
	printf("a = %p\n", a);

	int *c = my_malloc(200);
	
	printf("b = %p\n", c);
}