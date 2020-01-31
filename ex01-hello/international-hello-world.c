#include <stdio.h>
#include <string.h>
#include "international-hello-world.h"

int main(void) {
    print_hello_string(); 
    return 0;
}

void print_hello_string() {
    #ifdef FRENCH
    printf("Bonjour le monde\n");
    #endif

    #ifdef ENGLISH
    printf("hello world\n");
    #endif

    #ifdef CHINESE
    printf("Nihao, shijie\n");
    #endif    

    #ifdef DANISH
    printf("Hej Verden\n");
    #endif
}