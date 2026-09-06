#ifndef GOLOHI_UI_COUNTDOWN_H
#define GOLOHI_UI_COUNTDOWN_H

#include "app/app_state.h"
#include "lvgl/lvgl.h"

typedef struct {
    lv_obj_t * page;
    lv_obj_t * number;
} ui_countdown_t;

void ui_countdown_create(ui_countdown_t * ui, lv_obj_t * parent);
void ui_countdown_render(ui_countdown_t * ui, const app_state_t * state);

#endif
