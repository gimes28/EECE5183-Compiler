#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool getbool(){
    int b; 
    scanf("%i", &b);
    getchar();
    return b != 0;
}

bool putbool(bool b){
    printf("%i\n", b);
    return true;
}

int getinteger(){
    int i; 
    scanf("%i", &i);
    getchar();
    return i;
}

bool putinteger(int i){
    printf("%i\n", i);
    return true;
}

float getfloat(){
    float f; 
    scanf("%f", &f);
    getchar();
    return f;
}

bool putfloat(float f){
    printf("%f\n", f);
    return true;
}

char* getstring(){
    // Create buffer for incoming chars
    char* s = malloc(256 * sizeof(s)); 

    // Set s to input string
    fgets(s, 256, stdin);

    // Remove newline char
    if((strlen(s) > 0) && (s[strlen(s) - 1] == '\n'))
        s[strlen(s) - 1] = '\0';    
    
    // Get string has memory leak
    return s;
}

bool putstring(char* s){
    printf("%s\n", s);
    return true;
}

// Extended name due to conflicts with standard C library, can try another method if needed
float squareroot(int i){
    return sqrtf((float)i);
}

// Creating OutOfBoundsError call for runtime error to terminate LLVM
// Standard error call would not satisfy exiting LLVM before entering the next block
void outofboundserror(){
    // Perform exit(1) to tell LLVM to exit. Due to c not not having an unordered_map library 
    // to handle the errorTable, using a print statement will be used for the time being. 
    printf("Array index out of bounds\n");
    exit(1);
}