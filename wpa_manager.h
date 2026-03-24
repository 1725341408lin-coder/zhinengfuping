#ifndef _WPA_MANAGER_H_
#define _WPA_MANAGER_H_

#define STA_IFNAME "wlan0"
#define STA_CONFIG_PATH "/etc/wifi/wpa_supplicant/wpa_supplicant.conf"

typedef enum{
    WPA_WIFI_INACTIVE=0,
    WPA_WIFI_SCANNING,
    WPA_WIFI_DISCONNECT,
    WPA_WIFI_CONNECT,
    WPA_WIFI_WRONG_KEY,
}WPA_WIFI_CONNECT_STATUS_E;

typedef enum{
    WPA_WIFI_CLOSE=0,
    WPA_WIFI_OPEN,
}WPA_WIFI_STATUS_E;

typedef struct {
    char ssid[32];
    char psw[32];
}wpa_ctrl_wifi_info_t;

typedef void(* connect_status_callback_fun)(WPA_WIFI_CONNECT_STATUS_E status);
typedef void(* wifi_status_callback_fun)(WPA_WIFI_STATUS_E status);


int wpa_manager_open(void);

void wpa_manager_wifi_status(void);

int wpa_manager_wifi_connect(wpa_ctrl_wifi_info_t *wifi_info);

void wpa_manager_add_callback(wifi_status_callback_fun wifi_status_f,
                                connect_status_callback_fun connect_status_f);

void connect_wifi(const char *name,const char *psw);
#endif
