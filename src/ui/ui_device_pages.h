#ifndef GOLOHI_UI_DEVICE_PAGES_H
#define GOLOHI_UI_DEVICE_PAGES_H

#include "app/app_state.h"
#include "lvgl/lvgl.h"

typedef struct {
    lv_obj_t * boot_page;
    lv_obj_t * boot_bar;
    lv_obj_t * sleep_page;
    lv_obj_t * charging_page;
    lv_obj_t * charging_ring;
    lv_obj_t * charging_percent;
    lv_obj_t * powered_off_page;
} ui_device_pages_t;

void ui_device_pages_create(ui_device_pages_t * pages, lv_obj_t * parent);
void ui_device_pages_render(ui_device_pages_t * pages, const app_state_t * state);

#endif
