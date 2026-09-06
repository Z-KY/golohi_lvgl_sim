#ifndef GOLOHI_UI_RADAR_H
#define GOLOHI_UI_RADAR_H

#include "app/app_state.h"
#include "lvgl/lvgl.h"

typedef struct {
    lv_obj_t * page;
    lv_obj_t * ring;
    lv_obj_t * check;
    lv_obj_t * normal;
    lv_obj_t * warning;
} ui_radar_t;

void ui_radar_create(ui_radar_t * ui, lv_obj_t * parent);
void ui_radar_render(ui_radar_t * ui, const app_state_t * state);

#endif
