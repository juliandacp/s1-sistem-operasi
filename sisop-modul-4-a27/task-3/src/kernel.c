#include "std_lib.h"
#include "kernel.h"

void parseSegment(char* seg, char* cmd, char* args);
void handleEcho(char* args, char* output);
void handleGrep(char* pattern, char* input, char* output);
void handleWc(char* input, char* output);
void handleCommand(char* buf);
void intToStr(int n, char* str);

int main() {
    char buf[128];

    clearScreen();
    printString("SumbulOS - A27\n");

    while (true) {
        printString("$> ");
        readString(buf);
        printString("\n");

        if (strlen(buf) > 0) {
   
            handleCommand(buf);
        }
    }
}


void printString(char* str) {
       while (*str != '\0') {

        interrupt(0x10, (0x0E << 8) | (*str), 0, 0, 0);
        str++;

    }
}

void readString(char* buf) {
        int i;
    char ch;
    
    i = 0;
    while (1) {
        ch = (char)(interrupt(0x16, 0x0000, 0, 0, 0) & 0xFF);
        if (ch == '\r') {
            buf[i] = '\0';
            printString("\r\n");
            break;
        } else if (ch == '\b' && i > 0) {
            i--;
            printString("\b \b");
        } else if (ch >= ' ' && ch <= '~') {
            buf[i] = ch;
            i++;
            interrupt(0x10, (0x0E << 8) | ch, 0, 0, 0);
        }
    }
}

void clearScreen() {
	int i;
        interrupt(0x10, (0x06 << 8) | 0x00, 0x0007, 0x0000, 0x184F);
    interrupt(0x10, (0x02 << 8) | 0x00, 0x0000, 0x0000, 0x0000);
  for (i = 0; i < 2000; i++) {
        putInMemory(0xB800, i * 2, ' ');
        putInMemory(0xB800, i * 2 + 1, 0x07);
    }
}

void intToStr(int n, char* str) {
        int i;
    int j;
    char tmp[12];
    
    i = 0;
    if (n == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }
    while (n > 0) {
        tmp[i] = '0' + mod(n, 10);
        i++;
        n = div(n, 10);
    }
    for (j = 0; j < i; j++) {
        str[j] = tmp[i - 1 - j];
    }
    str[i] = '\0';
}

void parseSegment(char* seg, char* cmd, char* args) {
    int i;
    int j;
    i = 0;
    j = 0;
    while (seg[i] == ' ') i++;
    while (seg[i] != ' ' && seg[i] != '\0'){
	cmd[j]=seg[i];
	j++;
	i++;
    }

    cmd[j] = '\0';
    j = 0;
    while (seg[i] == ' ') i++;
    while (seg[i] != '\0'){
    args[j] = seg[i];
	j++;
	i++;
    }
    while (j > 0 && args[j - 1] == ' ') j--;
    args[j] = '\0';
}



void handleEcho(char* args, char* output) {
     strcpy(args, output);
}

void handleGrep(char* pattern, char* input, char* output) {
      int i;
      int j;
      int found;
        if (pattern[0] == '\0') {
        output[0] = '\0';
        return;
    }
    i = 0;
    found = 0;
    
    while (input[i] != '\0') {
        j = 0;
        while (pattern[j] != '\0' && input[i + j] != '\0' && pattern[j] == input[i + j]) {
            j++;
        }
        
        if (pattern[j] == '\0') {
            found = 1;
            break;
        }
        i++;
    }
    
    if (found) {
        strcpy(input, output);
    } else {
        output[0] = '\0';
    }
}

void handleWc(char* input, char* output) {
	int len;
	int lines;
	int words;
	int inWord;
	int chars;
	int i;
	int j;
	int pos;
	char lineStr[12];
	char wordStr[12];
	char charStr[12];
	len = 0;
	while (input[len] != '\0') len++;
	if (len > 0 && input[len-1] == '\n') {
	input[len-1] = '\0';
	len--;
	}

    if (len  == 0) {
        output[0] = '0';
        output[1] = ' ';
        output[2] = '0';
        output[3] = ' ';
        output[4] = '0';
        output[5] = '\0';
        return;
    }

    lines = 1;
    words = 0;
   inWord = 0;
    for (i = 0; i < len; i++) {
        if (input[i] == '\n') {
            lines++;
        }
	if (input[i] == ' ' || input[i] == '\t' || input[i] == '\n'){
	inWord = 0;
	}
	else {
	if (inWord == 0) {
	words++;
	inWord = 1;
	}
    }

 }
    chars = len;

    intToStr(lines, lineStr);
    intToStr(words, wordStr);
    intToStr(chars, charStr);

    pos = 0;
    for (j = 0; lineStr[j] != '\0'; j++){
	 output[pos] = lineStr[j];
	 pos++;
    }
    output[pos] = ' ';
    pos++;
    for (j = 0; wordStr[j] != '\0'; j++){
     output[pos] = wordStr[j];
     pos++;
    }
    output[pos] = ' ';
    pos++;
    for (j = 0; charStr[j] != '\0'; j++){
    output[pos] = charStr[j];
    pos++;
    }
    output[pos] = '\0';
}

void handleCommand(char* buf) {
    char segments[3][128];
    int segCount;
    int i;
    int j;
    int seg;
    char cmd [32];
    char args[128];
    char pipeIn[128];
    char pipeOut[128];

    segCount = 0;
    i=0;
    j=0;
    seg=0;

    clear((byte*)segments[0], 128);

    clear((byte*)segments[1], 128);

    clear((byte*)segments[2], 128);

    clear((byte*)pipeIn, 128);

    clear((byte*)pipeOut, 128);

    while (buf[i] != '\0' && seg < 3) {
        if (buf[i] == '|') {
	    segments[seg][j] = '\0';
	    seg++;
	    j = 0;
        } else {
            segments[seg][j] = buf[i];
            j++;
        }
	i++;
    }

    segments[seg][j] = '\0';
    segCount = seg + 1;

    for (i = 0; i < segCount; i++) {
        clear((byte*)cmd, 32);
        clear((byte*)args, 128);
        clear((byte*)pipeOut, 128);

        parseSegment(segments[i], cmd, args);

        if (strcmp(cmd, "echo")) {
            handleEcho(args, pipeOut);
        } else if (strcmp(cmd, "grep")) {
            handleGrep(args, pipeIn, pipeOut);
        } else if (strcmp(cmd, "wc")) {
            handleWc(pipeIn, pipeOut);
        } else {
            printString("Unknown command: ");
            printString(cmd);
            printString("\n");
            return;
        }

        strcpy(pipeOut, pipeIn);
    }

    if (strlen(pipeIn) > 0) {
        printString(pipeIn);
        printString("\n");
    } else {
        printString("NULL\n");
    }
}

