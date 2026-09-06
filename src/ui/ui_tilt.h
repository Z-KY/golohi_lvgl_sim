#ifndef GOLOHI_UI_TILT_H
#define GOLOHI_UI_TILT_H

#include "app/app_state.h"
#include "lvgl/lvgl.h"

typedef struct {
    lv_obj_t * page;
    lv_obj_t * ring;
    lv_obj_t * device;
    lv_obj_t * angle_label;
    lv_obj_t * check;
    lv_obj_t * normal;
    lv_obj_t * warning;
} ui_tilt_t;

void ui_tilt_create(ui_tilt_t * ui, lv_obj_t * parent);
void ui_tilt_render(ui_tilt_t * ui, const app_state_t * state);

#endif
