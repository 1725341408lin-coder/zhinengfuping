#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "page_conf.h"

#define PARAM_SAVE_PATH SAVE_PATH
#define PARAM_SAVE_PATH_MAX_LEN 300

   typedef struct 
{
    char name[10];
    int data1;
    char data2;
}file_save_test_data_t;

void test_read() {
    // 1. 先写入测试数据（会自动创建文件）
    file_save_test_data_t wdata;
    strncpy(wdata.name, "test_data", sizeof(wdata.name) - 1);
    wdata.name[sizeof(wdata.name) - 1] = '\0';
    wdata.data1 = 100;
    wdata.data2 = 50;
    
    if (file_param_write("testfile.cfg", &wdata, sizeof(wdata)) == 0) {
        printf("✓ Test file created successfully\n");
    }
}


int file_param_write(const char* name,void*data,int len){
    char file_name[PARAM_SAVE_PATH_MAX_LEN];
    char file_name_copy[PARAM_SAVE_PATH_MAX_LEN];
    int fd=-1;

    snprintf(file_name,sizeof(file_name),"%s%s",PARAM_SAVE_PATH,name);
    snprintf(file_name_copy,sizeof(file_name_copy),"%s%s",PARAM_SAVE_PATH,name);

    if((fd=open(file_name_copy,O_WRONLY|O_CREAT|O_TRUNC,0644))<0){
        printf("open file %s failed\n",file_name_copy);
        return -1;
    }
    ssize_t bytes_written=write(fd,data,len);
    if(bytes_written!=len){
        printf("failed to write to %s\n",file_name_copy);
        close(fd);
        return -1;
    }

    if(fsync(fd)!=0){
        printf("Failed to fsync %s\n",file_name_copy);
        close(fd);
        return -1;
    }
    if(rename(file_name_copy,file_name)!=0){
        printf("Failed to rename %s to %s\n", file_name_copy, file_name);
        close(fd);
        return -1;
    }
    return 0;
   }

   int file_param_read(const char* name,void* data,int len){
    char file_name[PARAM_SAVE_PATH_MAX_LEN];
    char file_name_copy[PARAM_SAVE_PATH_MAX_LEN];
    int fd=-1;

    // 构建文件路径
    snprintf(file_name, sizeof(file_name), "%s%s", PARAM_SAVE_PATH, name);
    snprintf(file_name_copy, sizeof(file_name_copy), "%s%s_copy", PARAM_SAVE_PATH, name);

    if(access(file_name,F_OK)!=0){
        if(access(file_name_copy,F_OK)!=0){
            printf("Neither %s nor %s exists\n", file_name, file_name_copy);
            return -1;
        }
        if(rename(file_name_copy,file_name)!=0){
             printf("Failed to rename %s to %s\n", file_name_copy, file_name);
            return -1;
        }
    }
    if((fd=open(file_name,O_RDONLY))<0){
        printf("Failed to open %s\n", file_name);
        return -1;
    }
    ssize_t bytes_read=read(fd,data,len);
    if (bytes_read > 0) {
        printf("Read %ld bytes from %s\n", bytes_read, file_name);
        close(fd);
        return 0;
    } 
    close(fd);
    return -1;
   }

void file_save_test(){
    test_read();
    file_save_test_data_t rdata;
    int ret = file_param_read("testfile.cfg",&rdata,sizeof(rdata));
    if(ret == 0)
        printf("rdata name = %s %d %d\n",rdata.name,rdata.data1,rdata.data2);

    file_save_test_data_t wdata;
    memcpy(wdata.name,"xiaozhi\0",strlen("xiaozhi\0"));
    wdata.data1 = 10;
    wdata.data2 = 20;
    file_param_write("param.cfg",&wdata,sizeof(wdata));

    int ret2 = file_param_read("param.cfg",&rdata,sizeof(rdata));
    if(ret2 == 0)
        printf("经测试，写入成功，修改后rdata name = %s %d %d\n",rdata.name,rdata.data1,rdata.data2);
}
