#ifndef GOLOHI_UI_VOLUME_H
#define GOLOHI_UI_VOLUME_H

#include "app/app_state.h"
#include "lvgl/lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    lv_obj_t * page;
    lv_obj_t * value_label;
    lv_obj_t * segments[4];
} ui_volume_t;

void ui_volume_create(ui_volume_t * volume, lv_obj_t * parent);
void ui_volume_render(ui_volume_t * volume, const app_state_t * state);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* GOLOHI_UI_VOLUME_H */
