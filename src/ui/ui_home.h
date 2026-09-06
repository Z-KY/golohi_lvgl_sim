#ifndef GOLOHI_UI_HOME_H
#define GOLOHI_UI_HOME_H

#include "app/app_state.h"
#include "lvgl/lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    lv_obj_t * page;
    lv_obj_t * card;
    lv_obj_t * card_name;
    lv_obj_t * battery_body;
    lv_obj_t * battery_fill;
    lv_obj_t * battery_text;
    lv_obj_t * card_icons[APP_HOME_CARD_COUNT];
    lv_obj_t * dots[APP_HOME_CARD_COUNT];
    lv_point_t pointer_press_start;
    bool pointer_press_active;
} ui_home_t;

void ui_home_create(ui_home_t * home, lv_obj_t * parent);
void ui_home_render(ui_home_t * home, const app_state_t * state);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* GOLOHI_UI_HOME_H */
