#ifndef GOLOHI_UI_THEME_H
#define GOLOHI_UI_THEME_H

#include "lvgl/lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

void ui_theme_init(void);

void ui_theme_apply_screen(lv_obj_t * obj);
void ui_theme_apply_page(lv_obj_t * obj);
void ui_theme_apply_statusbar(lv_obj_t * obj);
void ui_theme_apply_chip(lv_obj_t * obj);
void ui_theme_apply_mode_card(lv_obj_t * obj);
void ui_theme_apply_card_name(lv_obj_t * obj);
void ui_theme_set_home_card_variant(lv_obj_t * card, lv_obj_t * name,
                                    bool is_workout);
void ui_theme_apply_icon_line(lv_obj_t * obj);
void ui_theme_apply_icon_ground(lv_obj_t * obj);

lv_color_t ui_theme_color_accent(void);
lv_color_t ui_theme_color_muted(void);
lv_color_t ui_theme_color_muted_dark(void);
lv_color_t ui_theme_color_card_ink(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* GOLOHI_UI_THEME_H */
