
#ifndef _PAGE_CONF_H_
#define _PAGE_CONF_H_


//void init_page_test(void);
void init_page_main(void);
void init_page_setting(void);
void init_page_alarm(void);
void init_page_dialog(void);






#define SAVE_PATH "./app1_for_test_io/res/"  //用于测试iotest.c
void file_save_test();  //用于测试iotest.c
int file_param_write(const char* name,void*data,int len);//用于测试iotest.c
int file_param_read(const char* name,void* data,int len);//用于测试iotest.c


#endif