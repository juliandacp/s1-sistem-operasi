#ifndef __KERNEL_H__
#define __KERNEL_H__

#include "std_type.h"

extern void putInMemory(int segment, int address, char character);
extern int interrupt(int number, int AX, int BX, int CX, int DX);
extern void getRTCTime(byte* hours, byte* minutes, byte* seconds);
extern void getRTCDate(byte* century, byte* year, byte* month, byte* day);
extern int getTickCount(void);
extern void setVGAPalette(byte* palette);

void printString(char* str);
void printChar(char c);
void readString(char* buf);
void clearScreen();

void readSector(byte* buf, int sector);
void writeSector(byte* buf, int sector);

extern int g_boot_ticks;

#endif // __KERNEL_H__
