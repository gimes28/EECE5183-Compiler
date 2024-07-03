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
    char* s; 
    scanf("%s", s);
    getchar();
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