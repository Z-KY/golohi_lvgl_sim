#ifndef GOLOHI_UI_MODE_SELECT_H
#define GOLOHI_UI_MODE_SELECT_H

#include "app/app_state.h"
#include "lvgl/lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    lv_obj_t * page;
    lv_obj_t * items[APP_WORKOUT_MODE_COUNT];
} ui_mode_select_t;

void ui_mode_select_create(ui_mode_select_t * mode_select, lv_obj_t * parent);
void ui_mode_select_render(ui_mode_select_t * mode_select,
                           const app_state_t * state);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* GOLOHI_UI_MODE_SELECT_H */
