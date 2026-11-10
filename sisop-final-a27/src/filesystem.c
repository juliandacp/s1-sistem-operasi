#include "kernel.h"
#include "std_lib.h"
#include "filesystem.h"

void fsInit()
{
  struct map_fs map_fs_buf;
  int i = 0;

  readSector(&map_fs_buf, FS_MAP_SECTOR_NUMBER);
  for (i = 0; i < 32; i++)
    map_fs_buf.is_used[i] = true;
  for (i = 256; i < 512; i++)
    map_fs_buf.is_used[i] = true;
  writeSector(&map_fs_buf, FS_MAP_SECTOR_NUMBER);
}

// TODO: 4. Implement fsRead function
void fsRead(struct file_metadata *metadata, enum fs_return *status)
{
  struct node_fs node_fs_buf;
  struct data_fs data_fs_buf;
  /**
  * add local variable here
  * ...
  */


  int i;
  int found = -1;
   byte data_idx;

  readSector((byte*)&data_fs_buf, FS_DATA_SECTOR_NUMBER);
  readSector((byte*)&data_fs_buf + SECTOR_SIZE, FS_DATA_SECTOR_NUMBER + 1);
  readSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

  /**
  *  add your code here
  * ...
  */

    for (i = 0; i < FS_MAX_NODE; i++) {
    if (node_fs_buf.nodes[i].parent_index == metadata->parent_index &&
        strcmp(node_fs_buf.nodes[i].node_name, metadata->node_name)) {
      found = i;
      break;
    }
  }

    if (found == -1) {
    *status = FS_R_NODE_NOT_FOUND;
    return;
  }
 
  if (node_fs_buf.nodes[found].data_index == FS_NODE_D_DIR) {
    *status = FS_R_TYPE_IS_DIRECTORY;
    return;
  }

  metadata->filesize = 0;
  data_idx = node_fs_buf.nodes[found].data_index;

  for (i = 0; i < FS_MAX_SECTOR; i++) {
    int sector_num = data_fs_buf.datas[data_idx].sectors[i];
    if (sector_num == 0x00) break;
    
    readSector(metadata->buffer + (i * SECTOR_SIZE), sector_num);
    metadata->filesize += SECTOR_SIZE;
  }

  *status = FS_SUCCESS;  

}

// TODO: 5. Implement fsWrite function
void fsWrite(struct file_metadata *metadata, enum fs_return *status)
{
  struct map_fs map_fs_buf;
  struct node_fs node_fs_buf;
  struct data_fs data_fs_buf;
  



  int i;
  int j;
  int free_node_index = -1;
  int free_data_index = -1;
  int free_block_count = 0;
  int needed_blocks;

   readSector((byte*)&map_fs_buf, FS_MAP_SECTOR_NUMBER);
  readSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);
  readSector((byte*)&data_fs_buf, FS_DATA_SECTOR_NUMBER);
  readSector((byte*)&data_fs_buf + SECTOR_SIZE, FS_DATA_SECTOR_NUMBER + 1);

   for (i = 0; i < FS_MAX_NODE; i++) {
    if (node_fs_buf.nodes[i].parent_index == metadata->parent_index &&
        strcmp(node_fs_buf.nodes[i].node_name, metadata->node_name)) {
      *status = FS_W_NODE_ALREADY_EXISTS;
      return;
    }
  }

  for (i = 0; i < FS_MAX_NODE; i++) {
    if (node_fs_buf.nodes[i].node_name[0] == '\0') {
      free_node_index = i;
      break;
    }
  }
  if (free_node_index == -1) {
    *status = FS_W_NO_FREE_NODE;
    return;
  }

  if (metadata->filesize == 0) {
    strcpy(node_fs_buf.nodes[free_node_index].node_name, metadata->node_name);
    node_fs_buf.nodes[free_node_index].parent_index = metadata->parent_index;
    node_fs_buf.nodes[free_node_index].data_index = FS_NODE_D_DIR;

    writeSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);
    writeSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);
    
    *status = FS_SUCCESS;
    return;
  }

  for (i = 0; i < FS_MAX_DATA; i++) {
    if (data_fs_buf.datas[i].sectors[0] == 0x00) {
      free_data_index = i;
      break;
    }
  }
  if (free_data_index == -1) {
    *status = FS_W_NO_FREE_DATA;
    return;
  }

  for (i = 0; i < SECTOR_SIZE; i++) {
    if (map_fs_buf.is_used[i] == false) {
      free_block_count++;
    }
  }

  needed_blocks = div(metadata->filesize + SECTOR_SIZE - 1, SECTOR_SIZE);

  if (free_block_count < needed_blocks) {
    *status = FS_W_NOT_ENOUGH_SPACE;
    return;
  }

  strcpy(node_fs_buf.nodes[free_node_index].node_name, metadata->node_name);
  node_fs_buf.nodes[free_node_index].parent_index = metadata->parent_index;
  node_fs_buf.nodes[free_node_index].data_index = free_data_index;

  j = 0;
  for (i = 0; i < SECTOR_SIZE && j < needed_blocks; i++) {
    if (map_fs_buf.is_used[i] == false) {
      map_fs_buf.is_used[i] = true;
      data_fs_buf.datas[free_data_index].sectors[j] = (byte)i;
      writeSector(metadata->buffer + (j * SECTOR_SIZE), i);
      j++;
    }
  }

  for (; j < FS_MAX_SECTOR; j++) {
    data_fs_buf.datas[free_data_index].sectors[j] = 0x00;
  }

  writeSector((byte*)&map_fs_buf, FS_MAP_SECTOR_NUMBER);
  writeSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);
  writeSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);
  writeSector((byte*)&data_fs_buf, FS_DATA_SECTOR_NUMBER);
  writeSector((byte*)&data_fs_buf + SECTOR_SIZE, FS_DATA_SECTOR_NUMBER + 1);

  *status = FS_SUCCESS;

}

// TODO: 6. Implement fsDelete function
void fsDelete(struct file_metadata* metadata, enum fs_return* status) {
  struct map_fs map_fs_buf;
  struct node_fs node_fs_buf;
  struct data_fs data_fs_buf;

  
  int i;
  int j;
  int found = -1;
  byte data_idx;

  
  readSector((byte*)&map_fs_buf, FS_MAP_SECTOR_NUMBER);
  readSector((byte*)&data_fs_buf, FS_DATA_SECTOR_NUMBER);
  readSector((byte*)&data_fs_buf + SECTOR_SIZE, FS_DATA_SECTOR_NUMBER + 1);
  readSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);


  for (i = 0; i < FS_MAX_NODE; i++) {
    if (node_fs_buf.nodes[i].parent_index == metadata->parent_index &&
        strcmp(node_fs_buf.nodes[i].node_name, metadata->node_name)) {
      found = i;
      break;
    }
  }

  if (found == -1) {
    *status = FS_R_NODE_NOT_FOUND;
    return;
  }

  if (node_fs_buf.nodes[found].data_index == FS_NODE_D_DIR) {
    *status = FS_R_TYPE_IS_DIRECTORY;
    return;
  }

    data_idx = node_fs_buf.nodes[found].data_index;

  if (data_idx != FS_NODE_D_DIR) {
    for (j = 0; j < FS_MAX_SECTOR; j++) {
      int sector_num = data_fs_buf.datas[data_idx].sectors[j];
      if (sector_num == 0x00) break;
      
      map_fs_buf.is_used[sector_num] = false;
      data_fs_buf.datas[data_idx].sectors[j] = 0x00;
    }
  }

  for (j = 0; j < MAX_FILENAME; j++) {
    node_fs_buf.nodes[found].node_name[j] = '\0';
  }
  node_fs_buf.nodes[found].parent_index = 0x00;
  node_fs_buf.nodes[found].data_index = 0x00;

  writeSector((byte*)&map_fs_buf, FS_MAP_SECTOR_NUMBER);
  writeSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);
  writeSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);
  writeSector((byte*)&data_fs_buf, FS_DATA_SECTOR_NUMBER);
  writeSector((byte*)&data_fs_buf + SECTOR_SIZE, FS_DATA_SECTOR_NUMBER + 1);

  *status = FS_SUCCESS;  
  
}
