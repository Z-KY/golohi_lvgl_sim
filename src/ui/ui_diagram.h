#ifndef GOLOHI_UI_DIAGRAM_H
#define GOLOHI_UI_DIAGRAM_H

#include "app/app_state.h"
#include "lvgl/lvgl.h"

typedef struct {
    lv_obj_t * page;
    lv_obj_t * distance_label;
    lv_obj_t * height_label;
    lv_obj_t * target_label;
    lv_obj_t * progress;
    lv_obj_t * device;
} ui_diagram_t;

void ui_diagram_create(ui_diagram_t * ui, lv_obj_t * parent);
void ui_diagram_render(ui_diagram_t * ui, const app_state_t * state);

#endif
