#include <stdio.h>
#include <string.h>
#include "fancy-hello-world.h"

int main(void){
    char name[10];
    fgets(name, 10, stdin);
    char output[100];
    hello_string(name, output);
    return 0;
}

void hello_string(char* name, char* output){
    char str1[50] = "Hello World, hello "; 
    output = strcat(str1, name);
    printf("%s", output);
}
