#include<stdio.h>
#include<string.h>
#include<stdlib.h>

struct initrd_header{
    unsigned char magic;
    char name[64];
    unsigned int offset;
    unsigned int length;
};

//ramdisk格式
//[size, [header[0], header[1],...,header[63]], file_content[0]...]

//程序传入参数：<filename, filename in ramdisk>
int main(int argc, char** argv){
    int nheaders = (argc - 1) / 2;
    //ramdisk的header块有64项
    struct initrd_header headers[64];
    
    int offset = sizeof(struct initrd_header)*64 + sizeof(int);

    for(int i = 0; i < nheaders; ++i){
        printf("write file %s -> %s at 0x%x\n", argv[i*2 +1], argv[i*2+2], offset);
        
        headers[i].magic = 0xFF;
        strncpy(headers[i].name, argv[i*2+2], 64);
        headers[i].offset = offset;

        FILE* file = fopen(argv[i*2+1], "r");
        if(file == 0){
            printf("error: open file %s\n", argv[i*2+1]);
            return -1;
        }
        fseek(file, 0, SEEK_END);
        headers[i].length = ftell(file);
        offset  += headers[i].length;
        
        fclose(file);
    }

    FILE* initrd = fopen("./initrd.img", "w");

    fwrite(&nheaders, sizeof(int), 1, initrd);
    fwrite(headers, sizeof(struct initrd_header), 64, initrd);

    for(int i = 0; i < nheaders; ++i){
        FILE* file = fopen(argv[i*2+1], "r");
       
        unsigned char* buf = (unsigned char*)malloc(headers[i].length);
        fread(buf, headers[i].length, 1, file);
        fwrite(buf, headers[i].length, 1, initrd);
        
        free(buf);
        fclose(file);
    }
    
    fclose(initrd);
    return 0;
}
