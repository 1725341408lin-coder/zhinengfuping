#include <stdio.h>
#include<time.h>
#include "lvgl.h"
#include "image_conf.h"
#include "font_conf.h"
#include "http_manager.h"

static lv_obj_t *weather_label;
static char weather_info[100]= "";

//声明通用样式
static lv_style_t com_style;
// 保留全局标签引用
static lv_obj_t *time_label = NULL;
// 保留全局 timer 引用
static lv_timer_t *timer = NULL;  
//初始化通用样式
static void com_style_init(){
    //初始化样式
    lv_style_init(&com_style);
    //判断如果样式非空，那就先重置，再设置
    if(lv_style_is_empty(&com_style) == false)
        lv_style_reset(&com_style);
    //样式背景设置为黑色，圆角设置为0，边框宽度设置为0，填充区域设置为0
    lv_style_set_bg_color(&com_style,lv_color_hex(0x000000));
    lv_style_set_radius(&com_style,0);
    lv_style_set_border_width(&com_style,0);
    lv_style_set_pad_all(&com_style,0);
    lv_style_set_outline_width(&com_style,0);
}


static lv_obj_t * init_user_avatar(lv_obj_t *parent){
    //创建图像控件
    lv_obj_t *user_img = lv_img_create(parent);
    //设置显示的图像
    lv_img_set_src(user_img,GET_IMAGE_PATH("icon_user1.png"));
    //设置对齐：在父对象中左侧居中对齐
    lv_obj_set_align(user_img,LV_ALIGN_LEFT_MID);
    //设置偏移左侧35像素
    lv_obj_set_style_pad_left(user_img,35,LV_PART_MAIN);
    return user_img;
}

//封装字库获取函数
static void obj_font_set(lv_obj_t *obj,int type, uint16_t weight){
    lv_font_t* font = get_font(type, weight);
    if(font != NULL)
        lv_obj_set_style_text_font(obj, font, LV_PART_MAIN);
}

static lv_obj_t* init_time_view(lv_obj_t *parent){
    lv_obj_t* cont =lv_obj_create(parent);
    lv_obj_set_size(cont,240,LV_PCT(100));
    lv_obj_add_style(cont,&com_style,LV_PART_MAIN);

    time_label=lv_label_create(cont);
    obj_font_set(time_label,FONT_TYPE_CN,60);
    lv_label_set_text(time_label,"05:20");
    lv_obj_set_style_text_color(time_label,lv_color_hex(0xffffff),0);
    lv_obj_align(time_label,LV_ALIGN_TOP_MID,0,60);

     weather_label=lv_label_create(cont);
    obj_font_set(weather_label,FONT_TYPE_CN,24);
    lv_label_set_text(weather_label,"大连：晴 0°C");
    lv_obj_set_style_text_color(weather_label,lv_color_hex(0xffffff),0);
    lv_obj_align_to(weather_label,time_label,LV_ALIGN_OUT_BOTTOM_MID,0,10);
    return cont;
}


void timer_cb_func(lv_timer_t *timer) {
    if (time_label == NULL) return;  // 安全检查
    
    time_t seconds;
    struct tm *timeinfo;
    
    // 获取时间
    if (time(&seconds) == (time_t)-1) {
        return;  // 时间获取失败
    }
    
    // 转换时间（复制到本地结构体，更安全）
    struct tm local_tm;
    if (localtime_r(&seconds, &local_tm) == NULL) {
        return;  // 转换失败
    }
    
    // 更新时间显示和天气显示
    lv_label_set_text_fmt(time_label, "%02d:%02d", 
                          local_tm.tm_hour, local_tm.tm_min);
   
    lv_label_set_text(weather_label,weather_info);
}

static void update_weather_display(void *data) {
    if(weather_label != NULL) {
        lv_label_set_text(weather_label, weather_info);
    }
}
static void weather_callback_func(char *data){
    printf("---->%s\n",data);
    strncpy(weather_info, data, sizeof(weather_info) - 1);
    weather_info[sizeof(weather_info) - 1] = '\0';
    lv_async_call(update_weather_display, NULL);
}
static void init_timer(void) {
    timer = lv_timer_create(timer_cb_func, 1000, NULL);
}

/**
 * @brief 安全删除当前页面并重置样式
 * @param style 需要重置的样式指针
 * @note 只有在没有屏幕切换操作时才执行清理，避免在动画过程中破坏界面
 */
static void delete_current_page(lv_style_t *style)
{
    // 获取当前活动屏幕对象
    lv_obj_t * act_scr = lv_scr_act();
    
    // 获取当前屏幕所在的显示器对象
    // 显示器对象包含了屏幕切换的状态信息
    lv_disp_t * d = lv_obj_get_disp(act_scr);
    
    // 判断是否可以安全删除：
    // 1. d->prev_scr == NULL：没有上一个屏幕正在等待切换（没有进行中的屏幕切换动画）
    // 2. d->scr_to_load == NULL：没有待加载的新屏幕
    // 3. d->scr_to_load == act_scr：待加载的屏幕就是当前屏幕（特殊情况，也安全）
    if (d->prev_scr == NULL && (d->scr_to_load == NULL || d->scr_to_load == act_scr))
    {
        // 删除当前屏幕上的所有子对象
        // 注意：只删除子对象，保留屏幕对象本身
        lv_obj_clean(act_scr);
        
        // 重置传入的样式对象
        // 释放样式占用的所有属性内存，恢复到初始状态
        lv_style_reset(style);
    }
    // 如果条件不满足（有屏幕切换在进行中），则跳过清理
    // 这样可以避免在屏幕切换动画过程中破坏界面，防止程序崩溃
}

static void deinit_timer(){
    if(timer != NULL)
        lv_timer_del(timer);
    timer = NULL;
}


static void menu_click_event_cb(lv_event_t* e){
    char* menu_name=(char*)lv_event_get_user_data(e);
    printf("menu_name:%s\n",menu_name);
    deinit_timer();
    if(strcmp(menu_name,"番茄时钟")==0){
        delete_current_page(&com_style);
        init_page_alarm();
    }else if(strcmp(menu_name,"系统设置") == 0){
        delete_current_page(&com_style);
        init_page_setting();
    }
}

static lv_obj_t* init_item(lv_obj_t* parent,char* imgurl,char* str){
    lv_obj_t* cont=lv_obj_create(parent);
    lv_obj_set_size(cont,LV_SIZE_CONTENT,LV_SIZE_CONTENT);
    lv_obj_add_style(cont,&com_style,LV_PART_MAIN);
    lv_obj_add_flag(cont,LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(cont,LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_top(cont,60,LV_PART_MAIN);

    lv_obj_t* img=lv_img_create(cont);
    lv_img_set_src(img,imgurl);

    lv_obj_t* label=lv_label_create(cont);
    obj_font_set(label,FONT_TYPE_CN,20);
    lv_label_set_text(label,str);
    lv_obj_set_style_text_color(label,lv_color_hex(0xffffff),0);
    lv_obj_align_to(label,img,LV_ALIGN_OUT_BOTTOM_MID,0,10);

    lv_obj_add_event_cb(cont,menu_click_event_cb,LV_EVENT_CLICKED,lv_label_get_text(label));
    return cont;
}


static lv_obj_t * init_menu_list(lv_obj_t *parent){
    lv_obj_t* cont=lv_obj_create(parent);
    lv_obj_set_size(cont,750,LV_PCT(100));
    lv_obj_add_style(cont,&com_style,LV_PART_MAIN);
    lv_obj_set_flex_flow(cont,LV_FLEX_FLOW_ROW);
    lv_obj_set_style_bg_opa(cont,LV_OPA_0,LV_PART_MAIN);
   // 关键：设置滚动条模式为隐藏
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);

    //初始化菜单
    init_item(cont,GET_IMAGE_PATH("icon_menu_linkage.png"),"场景联动");
    init_item(cont,GET_IMAGE_PATH("icon_menu_wifi.png"),"WiFi设置");
    init_item(cont,GET_IMAGE_PATH("icon_menu_time.png"),"时间设置");
    init_item(cont,GET_IMAGE_PATH("icon_menu_tomato_time.png"),"番茄时钟");
    init_item(cont,GET_IMAGE_PATH("icon_menu_city.png"),"城市设置");
    init_item(cont,GET_IMAGE_PATH("icon_menu_setting.png"),"系统设置");
    init_item(cont,GET_IMAGE_PATH("icon_menu_about.png"),"关于");
    return cont;

}












//创建黑色页面
void init_page_main(void)
{
    //初始化样式
    com_style_init();
    //创建页面对象
    lv_obj_t * cont = lv_obj_create(lv_scr_act());
    //设置铺满父对象
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    //添加样式
    lv_obj_add_style(cont, &com_style, LV_PART_MAIN);
    //清除可滚动标志
    lv_obj_clear_flag(cont,LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * img = init_user_avatar(cont);
   

    lv_obj_t * info_view = init_time_view(cont);
    lv_obj_align_to(info_view,img,LV_ALIGN_OUT_RIGHT_MID,60,0);

    lv_obj_t * menu_list = init_menu_list(cont);
    lv_obj_align_to(menu_list,info_view,LV_ALIGN_OUT_RIGHT_MID,150,0);

    init_timer();
    http_set_weather_callback(weather_callback_func);
    tianqixianshi();
}