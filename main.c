#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "lvgl.h"
#include "font_conf.h"
#include "page_conf.h"
#include "wpa_manager.h"


extern void lv_port_disp_init(bool is_disp_orientation);
extern void lv_port_indev_init(void);

int main() {
    //LVGL框架初始化
    lv_init();
    //LVGL显示屏幕初始化
    lv_port_disp_init(true);
    //LVGL输入设备初始化
    lv_port_indev_init();
    //字体初始化
    font_init();




    //init_page_main();
    //init_page_setting();
    //init_page_alarm();
    //init_page_dialog();
    //file_save_test();
//初始化
    wpa_manager_open();
    const char *ssid = "8848";
    const char *password = "12345678lin";
//连接路由
    connect_wifi( ssid, password);


    while (1) {
        lv_task_handler();
        //延时，保证cpu占有率不会过高
        usleep(1000);
    }
    return 0;
}