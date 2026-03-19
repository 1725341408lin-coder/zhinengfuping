
#include <stdio.h>
#include "lvgl.h"
#include "image_conf.h"
#include "font_conf.h"


static lv_style_t com_style;

//初始化通用样式
static void com_style_init(){
    lv_style_init(&com_style);
    if(lv_style_is_empty(&com_style) == false)
        lv_style_reset(&com_style);
    lv_style_set_bg_color(&com_style,lv_color_hex(0x000000));
    lv_style_set_radius(&com_style,0);
    lv_style_set_border_width(&com_style,0);
    lv_style_set_outline_width(&com_style,0);
    lv_style_set_pad_all(&com_style,0);
}

//封装字库获取函数
static void obj_font_set(lv_obj_t *obj,int type, uint16_t weight){
    lv_font_t* font = get_font(type, weight);
    if(font != NULL)
        lv_obj_set_style_text_font(obj, font, LV_PART_MAIN);
}

lv_obj_t *  init_item_view(lv_obj_t * parent,const char * img_url,const char * str){
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont,LV_SIZE_CONTENT,LV_SIZE_CONTENT);
    lv_obj_add_style(cont,&com_style,LV_PART_MAIN);

    lv_obj_t * img = lv_img_create(cont);
    lv_img_set_src(img, img_url);
    lv_obj_align(img,LV_ALIGN_LEFT_MID,50,0);

    lv_obj_t *label = lv_label_create(cont);
    obj_font_set(label,FONT_TYPE_CN,20);
    lv_label_set_text(label, str);
    lv_obj_align_to(label,img,LV_ALIGN_OUT_RIGHT_MID,14,-5);
    lv_obj_set_style_text_color(label,lv_color_hex(0xffffff),LV_PART_MAIN);
    return cont;
}

lv_obj_t * init_info_view(lv_obj_t * parent){
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont,LV_SIZE_CONTENT,LV_SIZE_CONTENT);
    lv_obj_add_style(cont,&com_style,LV_PART_MAIN);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);

    init_item_view(cont,GET_IMAGE_PATH("icon_2.png"),"小智学长");
    init_item_view(cont,GET_IMAGE_PATH("icon_3.png"),"嵌入式长视频创作者");
    init_item_view(cont,GET_IMAGE_PATH("icon_4.png"),"xiaozhixuezhang.com");
    return cont;
}

lv_obj_t * init_project_view(lv_obj_t * parent){
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont,LV_SIZE_CONTENT,LV_SIZE_CONTENT);
    lv_obj_add_style(cont,&com_style,LV_PART_MAIN);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);

    lv_obj_t * img1 = lv_img_create(cont);
    lv_img_set_src(img1, GET_IMAGE_PATH("img_ai.png"));
    lv_obj_t * img2 = lv_img_create(cont);
    lv_img_set_src(img2, GET_IMAGE_PATH("img_arm.png"));
    lv_obj_t * img3 = lv_img_create(cont);
    lv_img_set_src(img3, GET_IMAGE_PATH("img_printer.png"));
    return cont;
}

lv_obj_t * init_btn(lv_obj_t * parent,const char * str){
    lv_obj_t * btn = lv_btn_create(parent);
    lv_obj_set_size(btn,132,59);
    lv_obj_t * label = lv_label_create(btn);
    obj_font_set(label,FONT_TYPE_CN,25);
    lv_label_set_text(label, str);
    // lv_obj_center(label);
    lv_obj_align(label,LV_ALIGN_CENTER,0,-5);
    return btn;
}

void init_page_test(){
    com_style_init();
    lv_obj_t * cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cont,LV_PCT(100),LV_PCT(100));
    lv_obj_add_style(cont,&com_style,LV_PART_MAIN);

    lv_obj_t * img = lv_img_create(cont);
    lv_img_set_src(img, GET_IMAGE_PATH("icon_bg.png"));
    lv_obj_align(img,LV_ALIGN_LEFT_MID,50,0);

    lv_obj_t * img_user = lv_img_create(cont);
    lv_img_set_src(img_user, GET_IMAGE_PATH("icon_user.png"));
    lv_obj_align_to(img_user,img,LV_ALIGN_OUT_RIGHT_MID,70,0);
    
    lv_obj_t * info_view = init_info_view(cont);
    lv_obj_align_to(info_view,img_user,LV_ALIGN_OUT_RIGHT_MID,50,0);

    lv_obj_t * item_project = init_item_view(cont,GET_IMAGE_PATH("icon_1.png"),"我的作品");
    lv_obj_align_to(item_project,info_view,LV_ALIGN_OUT_RIGHT_MID,43,0);

    lv_obj_t *project_view = init_project_view(cont);
    lv_obj_align_to(project_view,item_project,LV_ALIGN_OUT_RIGHT_MID,43,0);

    lv_obj_t *btn = init_btn(cont,"联系我");
    lv_obj_align_to(btn,project_view,LV_ALIGN_OUT_RIGHT_MID,37,0);

}