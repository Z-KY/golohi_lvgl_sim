#ifndef GOLOHI_UI_RESULT_H
#define GOLOHI_UI_RESULT_H

#include "app/app_state.h"
#include "lvgl/lvgl.h"

typedef struct {
    lv_obj_t * page;
    lv_obj_t * result_stats_row;
    lv_obj_t * reps_label;
    lv_obj_t * timer_label;
    lv_obj_t * plank_body;
} ui_result_t;

void ui_result_create(ui_result_t * ui, lv_obj_t * parent);
void ui_result_render(ui_result_t * ui, const app_state_t * state);

#endif
