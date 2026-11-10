#ifndef __SHELL_H__
#define __SHELL_H__

#include "std_type.h"

void shell();
void printCWD(byte cwd);
void parseCommand(char* buf, char* cmd, char arg[3][64]);
void printBCD(byte val);

void cd(byte* cwd, char* dirname);
void ls(byte cwd, char* dirname);
void mv(byte cwd, char* src, char* dst);
void cp(byte cwd, char* src, char* dst);
void cat(byte cwd, char* filename);
void mkdir(byte cwd, char* dirname);

/* New commands */
void echo(char* text);
void echoToFile(byte cwd, char* text, char* path, bool append);
void pwd(byte cwd);
void help();
void rm(byte cwd, char* filename);
void date();
void calc(char* expr);
void uptime();
void view(byte cwd, char* filename);

#endif // __SHELL_H__
