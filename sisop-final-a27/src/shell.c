#include "shell.h"
#include "kernel.h"
#include "std_lib.h"
#include "filesystem.h"

void shell() {
  char buf[64];
  char cmd[64];
  char arg[3][64];

  byte cwd = FS_NODE_P_ROOT;

  while (true) {
    printString("MengOS:");
    printCWD(cwd);
    printString("$ ");
    readString(buf);
    parseCommand(buf, cmd, arg);

    if (strcmp(cmd, "cd")) cd(&cwd, arg[0]);
    else if (strcmp(cmd, "ls")) ls(cwd, arg[0]);
    else if (strcmp(cmd, "mv")) mv(cwd, arg[0], arg[1]);
    else if (strcmp(cmd, "cp")) cp(cwd, arg[0], arg[1]);
    else if (strcmp(cmd, "cat")) cat(cwd, arg[0]);
    else if (strcmp(cmd, "mkdir")) mkdir(cwd, arg[0]);
    else if (strcmp(cmd, "rm")) rm(cwd, arg[0]);
    else if (strcmp(cmd, "clear")) clearScreen();
    else if (strcmp(cmd, "echo")) {
      if (strcmp(arg[1], ">"))       echoToFile(cwd, arg[0], arg[2], false);
      else if (strcmp(arg[1], ">>")) echoToFile(cwd, arg[0], arg[2], true);
      else                           echo(arg[0]);
    }
    else if (strcmp(cmd, "pwd")) pwd(cwd);
    else if (strcmp(cmd, "help")) help();
    else if (strcmp(cmd, "date")) date();
    else if (strcmp(cmd, "calc")) calc(arg[0]);
    else if (strcmp(cmd, "uptime")) uptime();
    else if (strcmp(cmd, "view")) view(cwd, arg[0]);
    else printString("Invalid command\n");
  }
}

// TODO: 7. Implement printCWD function
void printCWD(byte cwd) {
  struct node_fs node_fs_buf;

  byte stack[FS_MAX_NODE];
  int top, i;

  readSector((byte*)&node_fs_buf.nodes[0], FS_NODE_SECTOR_NUMBER);
  readSector((byte*)&node_fs_buf.nodes[32], FS_NODE_SECTOR_NUMBER + 1);

  if(cwd == FS_NODE_P_ROOT){
    printString("/");
    return;
  }

  top = 0;

  while(cwd != FS_NODE_P_ROOT){
    stack[top++] = cwd;
    cwd = node_fs_buf.nodes[cwd].parent_index;
  }

  printString("/");

  for(i = top - 1; i >= 0; i--){
    printString(node_fs_buf.nodes[stack[i]].node_name);
    if(i > 0) printString("/");
  }
}

// TODO: 7. Call/Encapsulate printCWD
void pwd(byte cwd) {
  printCWD(cwd);
  printString("\n");
}

// TODO: 8. Implement parseCommand function
void parseCommand(char* buf, char* cmd, char arg[3][64]) {
  int i, j, k;

  clear((byte*)cmd, 64);
  for(i = 0; i < 3; i++) clear((byte*)arg[i], 64);

  i = 0;
  j = 0;

  // Command
  while(buf[i] != ' ' && buf[i] != '\0') cmd[j++] = buf[i++];

  cmd[j] = '\0';

  // Arguments
  for(k = 0; k < 3; k++){
    while (buf[i] == ' ') i++;
    j = 0;
    while(buf[i] != ' ' && buf[i] != '\0') arg[k][j++] = buf[i++];
    arg[k][j] = '\0';
    if(buf[i] == '\0') break;
  }
}

// TODO: 9. Implement cd function
void cd(byte* cwd, char* dirname) {
  struct node_fs node_fs_buf;

  int i;

  readSector((byte*)&node_fs_buf.nodes[0], FS_NODE_SECTOR_NUMBER);
  readSector((byte*)&node_fs_buf.nodes[32], FS_NODE_SECTOR_NUMBER + 1);

  if(strcmp(dirname, "/")){
    *cwd = FS_NODE_P_ROOT;
    return;
  }

  if(strcmp(dirname, "..")){
    if(*cwd != FS_NODE_P_ROOT) *cwd = node_fs_buf.nodes[*cwd].parent_index;
    return;
  }

  for(i = 0; i < FS_MAX_NODE; i++){
    if(node_fs_buf.nodes[i].parent_index == *cwd && strcmp(node_fs_buf.nodes[i].node_name, dirname) && node_fs_buf.nodes[i].data_index == FS_NODE_D_DIR){
      *cwd = i;
      return;
    }
  }

  printString("Directory not found\n");
}

// TODO: 10. Implement ls function
void ls(byte cwd, char* dirname) {
  struct node_fs node_fs_buf;

  byte target;
  int i;
  bool found;

  readSector((byte*)&node_fs_buf.nodes[0], FS_NODE_SECTOR_NUMBER);
  readSector((byte*)&node_fs_buf.nodes[32], FS_NODE_SECTOR_NUMBER + 1);

  target = cwd;

  if(dirname[0] != '\0' && !strcmp(dirname, ".")){
    found = false;
    for(i = 0; i < FS_MAX_NODE; i++){
      if(node_fs_buf.nodes[i].parent_index == cwd && strcmp(node_fs_buf.nodes[i].node_name, dirname) && node_fs_buf.nodes[i].data_index == FS_NODE_D_DIR){
        target = i;
        found = true;
        break;
      }
    }

    if(!found){
      printString("Directory not found\n");
      return;
    }
  }

  for(i = 0; i < FS_MAX_NODE; i++){
    if(node_fs_buf.nodes[i].parent_index == target){
      printString(node_fs_buf.nodes[i].node_name);
      printString(" ");
    }
  }
  printString("\n");
}

// TODO: 11. Implement mv function
void mv(byte cwd, char * src, char * dst) {
  struct node_fs node_fs_buf;

  int i, j;
  int src_node_idx;
  byte new_parent;
  char new_name[MAX_FILENAME];
  bool found_src;
  bool found_dst_dir;

  src_node_idx = -1;
  new_parent = cwd;
  found_src = false;
  found_dst_dir = false;
  clear((byte*)new_name, MAX_FILENAME);

  if(src[0] == '\0' || dst[0] == '\0'){
    printString("Usage: mv <filename> <destination_path/new_name>\n");
    return;
  }

  readSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

  // Cari berkas asal (src) di bawah direktori aktif saat ini (cwd)
  for(i = 0; i < FS_MAX_NODE; i++){
    if(node_fs_buf.nodes[i].parent_index == cwd && node_fs_buf.nodes[i].node_name[0] != '\0' && strcmp(node_fs_buf.nodes[i].node_name, src)){
      if(node_fs_buf.nodes[i].data_index == FS_NODE_D_DIR){
        printString("mv: Directory moving is not supported\n");
        return;
      }
      src_node_idx = i;
      found_src = true;
      break;
    }
  }

  if(!found_src){
    printString("mv: Source file not found\n");
    return;
  }

  // Tujuan Root
  if(dst[0] == '/'){
    new_parent = FS_NODE_P_ROOT;
    j = 0;
    i = 1;
    while (dst[i] != '\0' && j < MAX_FILENAME - 1) new_name[j++] = dst[i++];
    new_name[j] = '\0';
  }

  // Tujuan Parent
  else if(dst[0] == '.' && dst[1] == '.' && dst[2] == '/'){
    if(cwd == FS_NODE_P_ROOT) new_parent = FS_NODE_P_ROOT;
    else new_parent = node_fs_buf.nodes[cwd].parent_index;
    j = 0;
    i = 3;
    while(dst[i] != '\0' && j < MAX_FILENAME - 1) new_name[j++] = dst[i++];
    new_name[j] = '\0';
  }
  
  // Tujuan Child
  else{
    char dir_target[64];
    clear((byte*)dir_target, 64);
    i = 0;
    while(dst[i] != '\0' && dst[i] != '/'){
      dir_target[i] = dst[i];
      i++;
    }
    dir_target[i] = '\0';
    if(dst[i] == '/'){
      i++;
      j = 0;
      while (dst[i] != '\0' && j < MAX_FILENAME - 1) new_name[j++] = dst[i++];
      new_name[j] = '\0';
      for(j = 0; j < FS_MAX_NODE; j++){
        if(node_fs_buf.nodes[j].parent_index == cwd && node_fs_buf.nodes[j].node_name[0] != '\0' && strcmp(node_fs_buf.nodes[j].node_name, dir_target)){
          if(node_fs_buf.nodes[j].data_index == FS_NODE_D_DIR){
            new_parent = (byte)j;
            found_dst_dir = true;
          }
          break;
        }
      }

      if(!found_dst_dir){
        printString("mv: Destination directory not found\n");
        return;
      }
    }
    
    // Rename
    else{
      strcpy(new_name, dir_target);
      new_parent = cwd;
    }
  }

  if(new_name[0] == '\0'){
    printString("mv: Invalid destination filename\n");
    return;
  }

  for(i = 0; i < FS_MAX_NODE; i++){
    if(node_fs_buf.nodes[i].parent_index == new_parent && node_fs_buf.nodes[i].node_name[0] != '\0' && strcmp(node_fs_buf.nodes[i].node_name, new_name)){
      printString("mv: Destination file already exists\n");
      return;
    }
  }

  node_fs_buf.nodes[src_node_idx].parent_index = new_parent;
  strcpy(node_fs_buf.nodes[src_node_idx].node_name, new_name);

  writeSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);
  writeSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);
}

// TODO: 12. Implement cp function
void cp(byte cwd, char* src, char* dst) {
  struct node_fs node_fs_buf;

  struct file_metadata file_buf;
  enum fs_return status;

  int i, j;
  byte new_parent;
  char new_name[MAX_FILENAME];
  bool found_dst_dir;

  new_parent = cwd;
  found_dst_dir = false;
  clear((byte*)new_name, MAX_FILENAME);

  if(src[0] == '\0' || dst[0] == '\0'){
    printString("Usage: cp <filename> <destination_path/new_name>\n");
    return;
  }

  file_buf.parent_index = cwd;
  strcpy(file_buf.node_name, src);
  fsRead(&file_buf, &status);

  if(status == FS_R_NODE_NOT_FOUND){
    printString("cp: Source file not found\n");
    return;
  }else if(status == FS_R_TYPE_IS_DIRECTORY){
    printString("cp: Copying a directory is not supported\n");
    return;
  }

  readSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

  // Tujuan Root
  if(dst[0] == '/'){
    new_parent = FS_NODE_P_ROOT;
    j = 0;
    i = 1;
    while (dst[i] != '\0' && j < MAX_FILENAME - 1) new_name[j++] = dst[i++];
    new_name[j] = '\0';
    if(new_name[0] == '\0') strcpy(new_name, src);
  }

  // Tujuan Parent
  else if(dst[0] == '.' && dst[1] == '.' && dst[2] == '/'){
    if(cwd == FS_NODE_P_ROOT) new_parent = FS_NODE_P_ROOT;
    else new_parent = node_fs_buf.nodes[cwd].parent_index;
    j = 0;
    i = 3;
    while (dst[i] != '\0' && j < MAX_FILENAME - 1) new_name[j++] = dst[i++];
    new_name[j] = '\0';
    if(new_name[0] == '\0') strcpy(new_name, src);
  }
  
  // Tujuan Child
  else{
    char dir_target[64];
    clear((byte*)dir_target, 64);
    i = 0;
    while(dst[i] != '\0' && dst[i] != '/'){
      dir_target[i] = dst[i];
      i++;
    }
    dir_target[i] = '\0';
    if(dst[i] == '/'){
      i++;
      j = 0;
      while (dst[i] != '\0' && j < MAX_FILENAME - 1) new_name[j++] = dst[i++];
      new_name[j] = '\0';
      if(new_name[0] == '\0') strcpy(new_name, src);
      for(j = 0; j < FS_MAX_NODE; j++){
        if(node_fs_buf.nodes[j].parent_index == cwd && node_fs_buf.nodes[j].node_name[0] != '\0' && strcmp(node_fs_buf.nodes[j].node_name, dir_target)) {
          if(node_fs_buf.nodes[j].data_index == FS_NODE_D_DIR){
            new_parent = (byte)j;
            found_dst_dir = true;
          }
          break;
        }
      }

      if(!found_dst_dir){
        printString("cp: Destination directory not found\n");
        return;
      }
    }

    // Duplikat
    else{
      strcpy(new_name, dir_target);
      new_parent = cwd;
    }
  }

  if(new_name[0] == '\0'){
    printString("cp: Invalid destination filename\n");
    return;
  }

  file_buf.parent_index = new_parent;
  strcpy(file_buf.node_name, new_name);

  fsWrite(&file_buf, &status);

  if(status == FS_W_NODE_ALREADY_EXISTS) printString("cp: Destination file already exists\n");
  else if (status == FS_W_NO_FREE_NODE) printString("cp: Cannot create file, no free node slot\n");
  else if (status == FS_W_NO_FREE_DATA) printString("cp: Cannot create file, no free data index slot\n");
  else if (status == FS_W_NOT_ENOUGH_SPACE) printString("cp: Cannot create file, not enough disk sectors\n");
}

// TODO: 13. Implement cat function
void cat(byte cwd, char* filename) {
  struct file_metadata metadata;
  enum fs_return status;

  int i;

  metadata.parent_index = cwd;
  metadata.filesize = 0;
  strcpy(metadata.node_name, filename);

  fsRead(&metadata, &status);

  for (i = 0; i < metadata.filesize; i++) printChar(metadata.buffer[i]);
  printString("\n");
}

// TODO: 14. Implement mkdir
void mkdir(byte cwd, char* dirname) {
  struct file_metadata metadata;
  enum fs_return status;

  if (dirname[0] == '\0') {
    printString("Usage: mkdir <dirname>\n");
    return;
  }

  metadata.parent_index = cwd;
  metadata.filesize = 0;
  strcpy(metadata.node_name, dirname);
  clear(metadata.buffer, FS_MAX_SECTOR * SECTOR_SIZE);

  fsWrite(&metadata, &status);

  if (status == FS_W_NODE_ALREADY_EXISTS) printString("mkdir: Directory already exists\n");
  else if (status == FS_W_NO_FREE_NODE) printString("mkdir: No free node\n");
}

// TODO: 15. Implement rm
void rm(byte cwd, char* filename) {
  struct file_metadata metadata;
  enum fs_return status;
  int i, len;
  char clean_name[MAX_FILENAME];

  if (filename[0] == '\0') {
    printString("Usage: rm <filename>\n");
    return;
  }

  len = 0;
  while (filename[len] != '\0') len++;  
  for (i = 0; i < len; i++) clean_name[i] = filename[i];
  clean_name[i] = '\0';

  metadata.parent_index = cwd;
  metadata.filesize = 0;
  strcpy(metadata.node_name, clean_name);

  fsDelete(&metadata, &status);

  if (status == FS_R_NODE_NOT_FOUND) printString("rm: File not found\n");
  else if (status == FS_R_TYPE_IS_DIRECTORY) printString("rm: Cannot remove a directory\n");
  else if (status == FS_SUCCESS) printString("rm: File deleted successfully\n");
  else printString("rm: Unknown error\n");
}

/* Helper: print a BCD-encoded byte as two decimal digits (e.g. 0x25 → "25") */
void printBCD(byte val) {
  char buf[3];
  buf[0] = '0' + ((val >> 4) & 0x0F);
  buf[1] = '0' + (val & 0x0F);
  buf[2] = '\0';
  printString(buf);
}

// TODO: 16. Implement uptime
void uptime() {
  int now, elapsed, total_secs, h, m, s;
  char buf[8];

  now = getTickCount();
  elapsed = now - g_boot_ticks;
  if (elapsed < 0) elapsed = -elapsed;

  total_secs = div(elapsed * 10, 182);
  h = div(total_secs, 3600);
  m = div(mod(total_secs, 3600), 60);
  s = mod(total_secs, 60);

  printString("Uptime: ");
  intToStr(h, buf); printString(buf);
  printString("h ");
  intToStr(m, buf); printString(buf);
  printString("m ");
  intToStr(s, buf); printString(buf);
  printString("s\n");
}

// TODO: 17. Implement date
void date() {
  byte century, year, month, day;
  byte hours, minutes, seconds;
  int h;

  getRTCDate(&century, &year, &month, &day);
  getRTCTime(&hours, &minutes, &seconds);

  h = ((hours >> 4) & 0x0F) * 10 + (hours & 0x0F);
  h = h + 7;
  if (h >= 24) h = h - 24;

  printBCD(day); printString("/");
  printBCD(month); printString("/");
  printBCD(century); printBCD(year); printString(" ");

  if (div(h, 10) == 0) printString("0");
  {
    char hbuf[8];
    intToStr(h, hbuf);
    printString(hbuf);
  }
  printString(":");
  printBCD(minutes); printString(":");
  printBCD(seconds); printString("\n");
}

//Calculator Helper
int g_calc_error = 0;
int calcExpr(char* s, int* pos); 
int calcFactor(char* s, int* pos) {
  int val;

  if (g_calc_error) return 0;
  while (s[*pos] == ' ') (*pos)++;

  /* unary minus */
  if (s[*pos] == '-') {
    (*pos)++;
    return -calcFactor(s, pos);
  }

  /* parenthesised sub-expression — recurse back to expr level */
  if (s[*pos] == '(') {
    (*pos)++;
    val = calcExpr(s, pos);
    if (g_calc_error) return 0;
    while (s[*pos] == ' ') (*pos)++;
    if (s[*pos] != ')') { g_calc_error = 1; return 0; }
    (*pos)++;
    return val;
  }

  /* integer literal */
  if (s[*pos] >= '0' && s[*pos] <= '9') {
    val = 0;
    while (s[*pos] >= '0' && s[*pos] <= '9') {
      val = val * 10 + (s[*pos] - '0');
      (*pos)++;
    }
    return val;
  }

  g_calc_error = 1;
  return 0;
}
int calcTerm(char* s, int* pos) {
  int left, right, ua, ub, sign;
  char op;

  if (g_calc_error) return 0;
  left = calcFactor(s, pos);

  while (!g_calc_error) {
    while (s[*pos] == ' ') (*pos)++;
    op = s[*pos];
    if (op != '*' && op != '/') break;
    (*pos)++;
    right = calcFactor(s, pos);
    if (g_calc_error) return 0;

    if (op == '*') {
      left = left * right;
    } else {
      if (right == 0) { g_calc_error = 2; return 0; }
      sign = 1; ua = left; ub = right;
      if (ua < 0) { sign = -sign; ua = -ua; }
      if (ub < 0) { sign = -sign; ub = -ub; }
      left = div(ua, ub);
      if (sign < 0) left = -left;
    }
  }
  return left;
}
int calcExpr(char* s, int* pos) {
  int left, right;
  char op;

  if (g_calc_error) return 0;
  left = calcTerm(s, pos);

  while (!g_calc_error) {
    while (s[*pos] == ' ') (*pos)++;
    op = s[*pos];
    if (op != '+' && op != '-') break;
    (*pos)++;
    right = calcTerm(s, pos);
    if (g_calc_error) return 0;

    if (op == '+') left = left + right;
    else           left = left - right;
  }
  return left;
}

// TODO: 18. Implement calc
void calc(char* expr) {
  int pos, result;
  char buf[16];

  if (expr[0] == '\0') {
    printString("Usage: calc <expr>  e.g. calc 3*(2+1)-4/2\n");
    return;
  }

  g_calc_error = 0;
  pos = 0;
  result = calcExpr(expr, &pos);

  while (expr[pos] == ' ') pos++;

  if (g_calc_error == 2) {
    printString("calc: division by zero\n");
  } else if (g_calc_error || expr[pos] != '\0') {
    printString("calc: syntax error\n");
  } else {
    intToStr(result, buf);
    printString(buf);
    printString("\n");
  }
}

// TODO: 19a. Implement echo
void echo(char* text) {
  printString(text);
  printString("\n");
}

// TODO: 19b. Implement echo yang menulis ke file
void echoToFile(byte cwd, char* text, char* path, bool append) {
  struct file_metadata metadata;
  enum fs_return status;
  unsigned int tlen, i;

  if (path[0] == '\0') {
    printString("Usage: echo <text> > <filename>\n");
    return;
  }

  tlen = strlen(text);

  if (append) {
    metadata.parent_index = cwd;
    metadata.filesize = 0;
    strcpy(metadata.node_name, path);
    clear(metadata.buffer, FS_MAX_SECTOR * SECTOR_SIZE);
    fsRead(&metadata, &status);

    if (status == FS_SUCCESS) {
      for (i = 0; i < tlen; i++) metadata.buffer[metadata.filesize + i] = text[i];
      metadata.filesize += tlen;
      fsDelete(&metadata, &status);
      strcpy(metadata.node_name, path);
      metadata.parent_index = cwd;
      fsWrite(&metadata, &status);
      return;
    }
  } else {
    metadata.parent_index = cwd;
    metadata.filesize = 0;
    strcpy(metadata.node_name, path);
    fsDelete(&metadata, &status);
  }

  clear(metadata.buffer, FS_MAX_SECTOR * SECTOR_SIZE);
  metadata.parent_index = cwd;
  strcpy(metadata.node_name, path);
  for (i = 0; i < tlen; i++) metadata.buffer[i] = text[i];
  metadata.filesize = tlen;

  fsWrite(&metadata, &status);

  if (status == FS_W_NODE_ALREADY_EXISTS) printString("echo: File already exists\n");
  else if (status == FS_W_NO_FREE_NODE) printString("echo: No free node\n");
  else if (status == FS_W_NO_FREE_DATA) printString("echo: No free data\n");
  else if (status == FS_W_NOT_ENOUGH_SPACE) printString("echo: Not enough space\n");
}

// TODO: 20. Implement help
void help() {
  printString("Available commands:\n");
  printString("  cd <dir>        - Change directory\n");
  printString("  ls [dir]        - List directory contents\n");
  printString("  mkdir <dir>     - Create directory\n");
  printString("  rm <file>       - Remove file\n");
  printString("  cp <src> <dst>  - Copy file\n");
  printString("  mv <src> <dst>  - Move file\n");
  printString("  cat <file>      - Display file contents\n");
  printString("  echo <text>     - Print text or write to file\n");
  printString("  calc <expr>     - Calculate expression\n");
  printString("  date            - Show current date and time\n");
  printString("  uptime          - Show system uptime\n");
  printString("  pwd             - Print working directory\n");
  printString("  clear           - Clear screen\n");
  printString("  help            - Show this help\n");
}

// TODO: Bonus (Optional). Implement view
 void view(byte cwd, char* filename) {
    struct file_metadata metadata;
  enum fs_return status;

  unsigned int width, height;
  unsigned int x_offset, y_offset;
  unsigned int x, y;
  unsigned int img_offset, vga_offset;
  byte color;
  int key;

  metadata.parent_index = cwd;
  metadata.filesize = 0;
  strcpy(metadata.node_name, filename);

  fsRead(&metadata, &status);

  if (status == FS_R_NODE_NOT_FOUND) {
    printString("view: file not found\n");
    return;
  }
  if (status == FS_R_TYPE_IS_DIRECTORY) {
    printString("view: target is a directory\n");
    return;
  }
  if (metadata.filesize < 773) {
    printString("view: invalid image file\n");
    return;
  }

  width  = metadata.buffer[0] | (metadata.buffer[1] << 8);
  height = metadata.buffer[2] | (metadata.buffer[3] << 8);

  if (width == 0 || width > 320 || height == 0 || height > 200) {
    printString("view: invalid image dimensions\n");
    return;
  }

  interrupt(0x10, 0x0013, 0, 0, 0);

  setVGAPalette(metadata.buffer + 4);

  x_offset = (320 - width) / 2;
  y_offset = (200 - height) / 2;

  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      img_offset = y * width + x;
      vga_offset = (y + y_offset) * 320 + (x + x_offset);
      color = metadata.buffer[772 + img_offset];
      putInMemory(0xA000, vga_offset, color);
    }
  }

  while (true) {
    key = interrupt(0x16, 0x0000, 0, 0, 0) & 0xFF;
    if (key == 'q' || key == 'Q' || key == 27) break;
  }

  interrupt(0x10, 0x0003, 0, 0, 0);
}
