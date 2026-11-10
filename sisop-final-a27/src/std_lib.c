#include "std_lib.h"

int div(int a, int b) {
  int res = 0;
  while (a >= b) {
    a -= b;
    res++;
  }
  return res;
}

int mod(int a, int b) {
  while (a >= b) a -= b;
  return a;
}

void memcpy(byte* dst, byte* src, unsigned int size) {
  unsigned int i;
  for (i = 0; i < size; i++) dst[i] = src[i];
}

unsigned int strlen(char* str) {
  unsigned int len = 0;
  while (str[len] != '\0') len++;
  return len;
}

bool strcmp(char* str1, char* str2) {
  unsigned int i = 0;
  while (str1[i] != '\0' && str2[i] != '\0') {
    if (str1[i] != str2[i]) return false;
    i++;
  }
  return str1[i] == str2[i];
}

void strcpy(char* dst, char* src) {
  unsigned int i = 0;
  while (src[i] != '\0') {
    dst[i] = src[i];
    i++;
  }
  dst[i] = '\0';
}

void clear(byte* buf, unsigned int size) {
  unsigned int i;
  for (i = 0; i < size; i++) buf[i] = 0;
}

void memset(byte* buf, byte val, unsigned int size) {
  unsigned int i;
  for (i = 0; i < size; i++) buf[i] = val;
}

/* Convert integer to decimal string.
 * buf must be at least 8 bytes. Handles negative values.
 * Uses custom div/mod to avoid C runtime division helpers. */
void intToStr(int n, char* buf) {
  int i, j;
  char tmp[8];
  i = 0; j = 0;

  if (n == 0) {
    buf[0] = '0';
    buf[1] = '\0';
    return;
  }

  if (n < 0) {
    buf[i++] = '-';
    n = -n;
  }

  while (n > 0) {
    tmp[j++] = '0' + mod(n, 10);
    n = div(n, 10);
  }

  while (j > 0) {
    buf[i++] = tmp[--j];
  }
  buf[i] = '\0';
}

/* Convert decimal string to integer. Handles optional leading '-'. */
int strToInt(char* str) {
  int result, sign, i;
  result = 0; i = 0; sign = 1;

  if (str[0] == '-') {
    sign = -1;
    i = 1;
  }

  while (str[i] >= '0' && str[i] <= '9') {
    result = result * 10 + (str[i] - '0');
    i++;
  }

  if (sign < 0) result = -result;
  return result;
}