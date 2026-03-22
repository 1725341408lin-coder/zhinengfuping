#include <stdio.h>
#include "lvgl.h"
#include "image_conf.h"
#include "font_conf.h"

static lv_obj_t *time_label;
//声明通用样式
static lv_style_t com_style;
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

//封装字库获取函数
static void obj_font_set(lv_obj_t *obj,int type, uint16_t weight){
    lv_font_t* font = get_font(type, weight);
    if(font != NULL)
        lv_obj_set_style_text_font(obj, font, LV_PART_MAIN);
}

static void btn_click_event_cb_func(lv_event_t * e){
    //获取layer top对象
    lv_obj_t * act_scr = lv_layer_top();
    lv_obj_clean(act_scr);
    lv_style_reset(&com_style);
}

static lv_obj_t * init_btn(lv_obj_t * parent,const char * str){
    lv_obj_t * btn = lv_btn_create(parent);
    //设置按钮样式
    lv_obj_add_style(btn,&com_style,LV_PART_MAIN);
    //设置按钮大小
    lv_obj_set_size(btn,150,66);
    //清除焦点状态
    lv_obj_clear_state(btn,LV_STATE_FOCUS_KEY);
    //设置边框、阴影为0
    lv_obj_set_style_border_width(btn, 0,LV_PART_MAIN);
    lv_obj_set_style_shadow_width(btn, 0,LV_PART_MAIN);
    //设置圆角为35
    lv_obj_set_style_radius(btn,35,LV_PART_MAIN);
    //设置背景颜色为蓝色
    lv_obj_set_style_bg_color(btn,lv_color_hex(0x1F94D2),0);
    lv_obj_add_event_cb(btn,btn_click_event_cb_func,LV_EVENT_CLICKED,NULL);

    lv_obj_t * label = lv_label_create(btn);
    obj_font_set(label,FONT_TYPE_CN,25);
    lv_label_set_text(label, str);
    // lv_obj_center(label);
    lv_obj_align(label,LV_ALIGN_CENTER,0,-5);
    return btn;
}

static void init_msg_view(lv_obj_t * parent){
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont,LV_SIZE_CONTENT,LV_SIZE_CONTENT);
    lv_obj_add_style(cont,&com_style,LV_PART_MAIN);
    lv_obj_center(cont);
    
    lv_obj_t * img = lv_img_create(cont);
    lv_img_set_src(img, GET_IMAGE_PATH("icon_alarm.png"));
    lv_obj_align(img,LV_ALIGN_TOP_MID,0,0);

    lv_obj_t *label = lv_label_create(cont);
    obj_font_set(label,FONT_TYPE_CN,20);
    lv_label_set_text(label, "嘿，闹铃时间到啦");
    lv_obj_align_to(label,img,LV_ALIGN_OUT_BOTTOM_MID,0,20);
    lv_obj_set_style_text_color(label,lv_color_hex(0xffffff),LV_PART_MAIN);

    lv_obj_t * btn = init_btn(cont,"确定");
    lv_obj_align_to(btn,label,LV_ALIGN_OUT_BOTTOM_MID,0,50);
}

void init_page_dialog(){
    com_style_init();
    lv_obj_t * cont = lv_obj_create(lv_layer_top());
    lv_obj_set_size(cont,LV_PCT(100),LV_PCT(100));
    lv_obj_add_style(cont,&com_style,LV_PART_MAIN);

    init_msg_view(cont);
}