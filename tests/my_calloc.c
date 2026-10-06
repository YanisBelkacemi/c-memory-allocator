#include <stdio.h>
#include "../headers/allocator.h"


int main(void){
    malloc_init();
	int *a = my_calloc(100);

	printf("a = %p\n", a);
	printf("a as a value = %d\n", *a);
}