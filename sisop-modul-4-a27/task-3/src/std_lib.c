#include "std_lib.h"


int div(int a, int b) {
	int quotient;
	int sign;
	int absA;
	int absB;
         if (b == 0) return 0;
    
    quotient = 0;
    sign = 1;
    
    if ((a < 0 && b > 0) || (a > 0 && b < 0)) {
        sign = -1;
    }
    
    absA = a < 0 ? -a : a;
    absB = b < 0 ? -b : b;
    
    while (absA >= absB) {
        absA -= absB;
        quotient++;
    }
    
    return sign * quotient;
}

int mod(int a, int b) {
    if (b == 0) return 0;
    return a - (div(a, b) * b);
}

void memcpy(byte* src, byte* dst, unsigned int size) {
 unsigned int i;  
  for (i = 0; i < size; i++) {
    dst[i] = src[i];
    }
}

unsigned int strlen(char* str) {
    unsigned int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

bool strcmp(char* str1, char* str2) {
        int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            return 0;
        }
        i++;
    }
    return (str1[i] == '\0' && str2[i] == '\0');
}

void strcpy(char* src, char* dst) {
    int i = 0;
    while (src[i] != '\0') {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}

void clear(byte* buf, unsigned int size) {
	unsigned int i;   
 for (i = 0; i < size; i++) {
    buf[i] = 0;
    }
}
