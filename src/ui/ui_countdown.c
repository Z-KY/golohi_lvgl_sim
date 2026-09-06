#include "ui/ui_countdown.h"

#include <stddef.h>

#include "ui/ui_theme.h"

void ui_countdown_create(ui_countdown_t * ui, lv_obj_t * parent)
{
    if(ui == NULL || parent == NULL) return;
    ui->page = lv_obj_create(parent);
    ui_theme_apply_page(ui->page);
    lv_obj_set_size(ui->page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(ui->page, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui->page, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(ui->page, 16, LV_PART_MAIN);

    ui->number = lv_label_create(ui->page);
    lv_label_set_text(ui->number, "3");
    lv_obj_set_style_text_color(ui->number, ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_text_font(ui->number, &lv_font_montserrat_32, LV_PART_MAIN);
    lv_obj_set_style_transform_zoom(ui->number, 420, LV_PART_MAIN);

    lv_obj_t * caption = lv_label_create(ui->page);
    lv_label_set_text(caption, "Get ready");
    lv_obj_set_style_text_color(caption, ui_theme_color_muted(), LV_PART_MAIN);
    lv_obj_set_style_text_font(caption, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(caption, 2, LV_PART_MAIN);
}

void ui_countdown_render(ui_countdown_t * ui, const app_state_t * state)
{
    char number[4];
    if(ui == NULL || state == NULL) return;
    lv_snprintf(number, sizeof(number), "%u", state->countdown);
    lv_label_set_text(ui->number, number);
}
