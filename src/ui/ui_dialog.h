#ifndef GOLOHI_UI_DIALOG_H
#define GOLOHI_UI_DIALOG_H

#include "app/app_state.h"
#include "lvgl/lvgl.h"

typedef struct {
    lv_obj_t * overlay;
    lv_obj_t * sheet;
    lv_obj_t * icon_circle;
    lv_obj_t * icon;
    lv_obj_t * title;
    lv_obj_t * message;
    lv_obj_t * ok_button;
    lv_obj_t * cancel_button;
    lv_obj_t * power_off_button;
} ui_dialog_t;

void ui_dialog_create(ui_dialog_t * dialog, lv_obj_t * parent);
void ui_dialog_render(ui_dialog_t * dialog, const app_state_t * state);

#endif
