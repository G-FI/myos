#ifndef INITRD_H_
#define INITRD_H_

#include<stdint.h>
#include "fs.h"

typedef struct
{
   uint32_t nfiles; // The number of files in the ramdisk.
} initrd_header_t;

typedef struct
{
   uint8_t magic;     
   char name[64];  //文件名
   uint32_t offset;   // ramdisk该文件起始的位置
   uint32_t length;   //文件长度
} initrd_file_header_t;

//初始化其实内存文件系统，返回一个文件系统节点
fs_node_t *initialise_initrd(uint32_t location);


#endif //INITRD_H_