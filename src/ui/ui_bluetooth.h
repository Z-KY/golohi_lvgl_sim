#ifndef GOLOHI_UI_BLUETOOTH_H
#define GOLOHI_UI_BLUETOOTH_H

#include "app/app_state.h"
#include "lvgl/lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    lv_obj_t * page;
    lv_obj_t * ring;
    lv_obj_t * spinner;
    lv_obj_t * icon;
    lv_obj_t * state_label;
    lv_obj_t * action_button;
    lv_obj_t * action_label;
} ui_bluetooth_t;

void ui_bluetooth_create(ui_bluetooth_t * bluetooth, lv_obj_t * parent);
void ui_bluetooth_render(ui_bluetooth_t * bluetooth,
                         const app_state_t * state);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* GOLOHI_UI_BLUETOOTH_H */
