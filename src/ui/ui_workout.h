#ifndef GOLOHI_UI_WORKOUT_H
#define GOLOHI_UI_WORKOUT_H

#include "app/app_state.h"
#include "lvgl/lvgl.h"

typedef struct {
    lv_obj_t * page;
    lv_obj_t * mode_label;
    lv_obj_t * primary;
    lv_obj_t * timer;
    lv_obj_t * tip;
    lv_obj_t * pause_label;
} ui_workout_t;

void ui_workout_create(ui_workout_t * ui, lv_obj_t * parent);
void ui_workout_render(ui_workout_t * ui, const app_state_t * state);

#endif
