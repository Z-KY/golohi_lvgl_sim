#ifndef GOLOHI_UI_BRIGHTNESS_H
#define GOLOHI_UI_BRIGHTNESS_H

#include "app/app_state.h"
#include "lvgl/lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    lv_obj_t * page;
    lv_obj_t * value_label;
    lv_obj_t * segments[4];
} ui_brightness_t;

void ui_brightness_create(ui_brightness_t * brightness, lv_obj_t * parent);
void ui_brightness_render(ui_brightness_t * brightness,
                          const app_state_t * state);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* GOLOHI_UI_BRIGHTNESS_H */
