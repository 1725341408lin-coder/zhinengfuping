#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "common/wpa_ctrl.h"
#include "osal_thread.h"
#include "wpa_manager.h"

static osal_thread_t event_thread=NULL;

static struct wpa_ctrl* g_pstWpaCtrl=NULL;

static WPA_WIFI_STATUS_E g_wifi_status=WPA_WIFI_CLOSE;

static WPA_WIFI_CONNECT_STATUS_E g_connect_status=WPA_WIFI_INACTIVE;

connect_status_callback_fun connect_status_func=NULL;

wifi_status_callback_fun wifi_status_func=NULL;

static int wifi_send_command(const char * cmd,char* reply,size_t* reply_len){
    int ret;
    if(g_pstWpaCtrl==NULL){
        printf("Not connected to wpa_supplicant - \"%s\" command dropped.\n", cmd);
        return -1;
    }
    printf("发送命令: %s\n", cmd);
    ret=wpa_ctrl_request(g_pstWpaCtrl,cmd,strlen(cmd),reply,reply_len,NULL);

    if(ret<0){
        printf("'%s' command error.\n", cmd);
        return ret;
    }
    
    if(reply && *reply_len > 0){
        reply[*reply_len] = '\0';
        printf("'%s' 回复: %s\n", cmd, reply);
    }
    return 0;
}

static void wpa_manager_wifi_on(){
    printf("wpa_manager_wifi_on\n");
    char cmdstr[200];
    
    sprintf(cmdstr,"ifconfig %s up",STA_IFNAME);
    printf("执行: %s\n", cmdstr);
    int ret = system(cmdstr);
    if(ret != 0){
        printf("警告: ifconfig %s up 失败\n", STA_IFNAME);
    }
    
    sprintf(cmdstr,"killall wpa_supplicant 2>/dev/null");
    system(cmdstr);
    osal_thread_sleep(500);
    
    sprintf(cmdstr,"rm -f /etc/wifi/wpa_supplicant/sockets/%s 2>/dev/null", STA_IFNAME);
    printf("清理旧socket文件: %s\n", cmdstr);
    system(cmdstr);
    
    sprintf(cmdstr,"wpa_supplicant -i %s -c %s -B -dd",STA_IFNAME,STA_CONFIG_PATH);
    printf("执行: %s\n", cmdstr);
    ret = system(cmdstr);
    if(ret != 0){
        printf("警告: wpa_supplicant启动可能失败\n");
    }
    
    g_wifi_status = WPA_WIFI_OPEN;
    if(wifi_status_func != NULL)
        wifi_status_func(g_wifi_status);

}

static int wpa_manager_connect_socket(void){
    char path[128];
    snprintf(path,sizeof(path),"/etc/wifi/wpa_supplicant/sockets/%s",STA_IFNAME);
    printf("尝试连接socket路径: %s\n", path);
    g_pstWpaCtrl=wpa_ctrl_open(path);
    if(g_pstWpaCtrl==NULL){
        printf("无法打开wpa_ctrl: %s\n", path);
        return -1;
    }

    int ret = wpa_ctrl_attach(g_pstWpaCtrl);
    if(ret != 0){
        printf("无法attach到wpa_ctrl\n");
        wpa_ctrl_close(g_pstWpaCtrl);
        g_pstWpaCtrl = NULL;
        return -1;
    }
    printf("成功连接到wpa_supplicant\n");
    return 0;

}

void wpa_manager_wifi_save_config(void){
     printf("wpa_manager_wifi_save_config\n");
    char reply_buf[256] = {0};
    size_t reply_len = sizeof(reply_buf);
    if (wifi_send_command("SAVE_CONFIG", reply_buf, &reply_len) == 0) {
        printf("SAVE_CONFIG--->status = %s\n",reply_buf);
    }
}


void wpa_manager_wifi_status(void){
     printf("wpa_manager_wifi_status\n");
     char reply_buf[256]={0};
     size_t reply_len=sizeof(reply_buf);

     if(wifi_send_command("STATUS",reply_buf,&reply_len)==0){
        if(strstr(reply_buf,"COMPLETED")!=NULL){
             g_connect_status = WPA_WIFI_CONNECT;
        }else if (strstr(reply_buf, "DISCONNECTED") != NULL) {
            g_connect_status = WPA_WIFI_DISCONNECT;
        } else if (strstr(reply_buf, "SCANNING") != NULL) {
            g_connect_status = WPA_WIFI_SCANNING;
        } else if (strstr(reply_buf, "INACTIVE") != NULL) {
            g_connect_status = WPA_WIFI_INACTIVE;
        }
     }
     printf("--->status = %d\n",g_connect_status);
     if(connect_status_func !=NULL){
        connect_status_func(g_connect_status);
     }
}

int wpa_manager_wifi_connect(wpa_ctrl_wifi_info_t* wifi_info){
    char reply_buf[2048] = {0};
    size_t reply_len;
    int ret;
    int net_id=-1;

    for (int count = 0; count < 10; count++){
        if(g_pstWpaCtrl != NULL) break;
        osal_thread_sleep(500);
        printf("等待 wpa_supplicant 连接...\n");
    }
    if(g_pstWpaCtrl == NULL){
        printf("wpa_supplicant 未连接，无法配置 WiFi\n");
        return -1;
    }

    printf("开始配置 WiFi...\n");
    reply_len=sizeof(reply_buf);
    wifi_send_command("REMOVE_NETWORK all",reply_buf,&reply_len);
    reply_len=sizeof(reply_buf);
    wifi_send_command("SAVE_CONFIG",reply_buf,&reply_len);
    
    reply_len=sizeof(reply_buf);
    printf("执行 ADD_NETWORK...\n");
    ret=wifi_send_command("ADD_NETWORK",reply_buf,&reply_len);
    if (ret != 0) {
        printf("ADD_NETWORK 失败\n");
        return ret;
    }
    reply_buf[reply_len]='\0';
    net_id=atoi(reply_buf);
    printf("网络 ID: %d\n", net_id);

    char cmd_buf[128];
    //如果想在字符串内容中包含双引号，就需要用 \" 进行转义。
    reply_len=sizeof(reply_buf);
    snprintf(cmd_buf,sizeof(cmd_buf),"SET_NETWORK %d ssid \"%s\"",net_id,wifi_info->ssid);
    ret=wifi_send_command(cmd_buf, reply_buf, &reply_len);
    if (ret != 0) {
        return ret;
    }
    // 设置PSK密码
    reply_len=sizeof(reply_buf);
    snprintf(cmd_buf, sizeof(cmd_buf), "SET_NETWORK %d psk \"%s\"", net_id, wifi_info->psw);
    ret = wifi_send_command(cmd_buf, reply_buf, &reply_len);
    if (ret != 0) {
        return ret;
    }

    reply_len=sizeof(reply_buf);
    snprintf(cmd_buf,sizeof(cmd_buf),"ENABLE_NETWORK %d",net_id);
    ret = wifi_send_command(cmd_buf, reply_buf, &reply_len);
    if (ret != 0) {
        return ret;
    }

    // 选择网络
    reply_len=sizeof(reply_buf);
    snprintf(cmd_buf, sizeof(cmd_buf), "SELECT_NETWORK %d", net_id);
    ret = wifi_send_command(cmd_buf, reply_buf, &reply_len);
    if (ret != 0) {
        return ret;
    }
    
    // 触发重新连接
    printf("触发重新连接...\n");
    reply_len=sizeof(reply_buf);
    wifi_send_command("REASSOCIATE", reply_buf, &reply_len);
    
    // 保存配置
    reply_len=sizeof(reply_buf);
    ret = wifi_send_command("SAVE_CONFIG", reply_buf, &reply_len);
    return ret;
}


void* wpa_manager_event_thread(void* arg){
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,NULL);

    wpa_manager_wifi_on();

    for (int count = 0; count < 10; count++){
        if(wpa_manager_connect_socket()==0) break;
        osal_thread_sleep(1000);
        printf("wpa_manager_connect_socket\n");
    }
    wpa_manager_wifi_status();
    while(1){
        if(g_pstWpaCtrl&&wpa_ctrl_pending(g_pstWpaCtrl)>0){
            char buf[512];
            size_t len=sizeof(buf);

            if(wpa_ctrl_recv(g_pstWpaCtrl,buf,&len)==0){
                buf[len]='\0';

                if(strstr(buf,"CTRL-EVENT-CONNECTED")){
                    char cmd[64];
                    snprintf(cmd,sizeof(cmd),"udhcpc -i %s -t 5 -T 2 -A 5 -q",STA_IFNAME);
                    system(cmd);
                    wpa_manager_wifi_save_config();
                    g_connect_status=WPA_WIFI_CONNECT;
                }else if(strstr(buf, "CTRL-EVENT-DISCONNECTED") != NULL){
                    g_connect_status = WPA_WIFI_DISCONNECT;
                } else if (strstr(buf, "CTRL-EVENT-SSID-TEMP-DISABLED")) {
                    g_connect_status = WPA_WIFI_WRONG_KEY;
                }
                printf("--->status = %d\n",g_connect_status);
                if(connect_status_func != NULL)
                    connect_status_func(g_connect_status);
            }
            
        }
        osal_thread_sleep(10);
    }
    return NULL;
}

void wpa_manager_add_callback(wifi_status_callback_fun wifi_status_f,connect_status_callback_fun connect_status_f){
    wifi_status_func = wifi_status_f;                       
    connect_status_func = connect_status_f;
}






// 内部连接状态回调函数（不暴露给外部）
static void internal_connect_status_callback(WPA_WIFI_CONNECT_STATUS_E status) {
    switch(status) {
        case WPA_WIFI_CONNECT:
            printf("[WiFi Manager] ✓ 连接成功！\n");
            // 可以在这里添加连接成功后的处理
            break;
        case WPA_WIFI_DISCONNECT:
            printf("[WiFi Manager] ✗ WiFi已断开\n");
            // 可以在这里添加断开后的处理（如自动重连）
            break;
        case WPA_WIFI_WRONG_KEY:
            printf("[WiFi Manager] ✗ 密码错误！\n");
            break;
        case WPA_WIFI_SCANNING:
            printf("[WiFi Manager] 正在扫描...\n");
            break;
        case WPA_WIFI_INACTIVE:
            printf("[WiFi Manager] WiFi未激活\n");
            break;
        default:
            break;
    }
}

static void internal_wifi_status_callback(WPA_WIFI_STATUS_E status) {
    if(status == WPA_WIFI_OPEN) {
        printf("[WiFi Manager] WiFi已开启\n");
    } else {
        printf("[WiFi Manager] WiFi已关闭\n");
    }
}


//初始化WiFi，创建WiFi线程
int wpa_manager_open(){


    wpa_manager_add_callback(internal_wifi_status_callback,internal_connect_status_callback);


    int ret = osal_thread_create(&event_thread,wpa_manager_event_thread, NULL);
    if(ret == -1)
    {
        printf("create thread error\n");
        return ret;
    }
    return ret;
}

void connect_wifi(const char *name,const char *psw){
     wpa_ctrl_wifi_info_t wifi_info;
    if(strlen(name) > 32 || strlen(psw) > 32){
        printf("wifi str len error\n");
        return;
    }
    memset(wifi_info.ssid,'\0',sizeof(wifi_info.ssid));
    memset(wifi_info.psw,'\0',sizeof(wifi_info.psw));
    memcpy(wifi_info.ssid,name,strlen(name));
    memcpy(wifi_info.psw,psw,strlen(psw));
    wpa_manager_wifi_connect(&wifi_info);
 }