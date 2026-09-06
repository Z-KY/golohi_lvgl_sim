#include "ui/ui_tilt.h"

#include <stddef.h>

#include "ui/ui_theme.h"

static void make_plain(lv_obj_t * obj)
{
    lv_obj_remove_style_all(obj);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
}

static lv_obj_t * create_center_label(lv_obj_t * parent, const char * text,
                                      int32_t y, const lv_font_t * font,
                                      lv_color_t color)
{
    lv_obj_t * label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, font, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, color, LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, y);
    return label;
}

void ui_tilt_create(ui_tilt_t * ui, lv_obj_t * parent)
{
    if(ui == NULL || parent == NULL) return;
    ui->page = lv_obj_create(parent);
    ui_theme_apply_page(ui->page);
    lv_obj_set_size(ui->page, LV_PCT(100), LV_PCT(100));
    create_center_label(ui->page, "TILT", 12, &lv_font_montserrat_12,
                        ui_theme_color_muted());

    ui->normal = lv_obj_create(ui->page);
    make_plain(ui->normal);
    lv_obj_set_size(ui->normal, 150, 160);
    lv_obj_align(ui->normal, LV_ALIGN_CENTER, 0, 10);

    ui->ring = lv_arc_create(ui->normal);
    lv_obj_set_size(ui->ring, 124, 124);
    lv_obj_align(ui->ring, LV_ALIGN_TOP_MID, 0, 0);
    lv_arc_set_rotation(ui->ring, 270);
    lv_arc_set_bg_angles(ui->ring, 0, 360);
    lv_arc_set_range(ui->ring, 0, 100);
    lv_obj_set_style_arc_width(ui->ring, 7, LV_PART_MAIN | LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(ui->ring, lv_color_hex(0x1B2430), LV_PART_MAIN);
    lv_obj_set_style_arc_color(ui->ring, ui_theme_color_accent(), LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(ui->ring, LV_OPA_TRANSP, LV_PART_KNOB);
    lv_obj_clear_flag(ui->ring, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t * stand = lv_obj_create(ui->normal);
    make_plain(stand);
    lv_obj_set_size(stand, 58, 3);
    lv_obj_set_pos(stand, 46, 92);
    lv_obj_set_style_bg_color(stand, ui_theme_color_muted_dark(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(stand, LV_OPA_COVER, LV_PART_MAIN);

    ui->device = lv_obj_create(ui->normal);
    make_plain(ui->device);
    lv_obj_set_size(ui->device, 44, 16);
    lv_obj_set_pos(ui->device, 53, 75);
    lv_obj_set_style_border_color(ui->device, ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_border_width(ui->device, 3, LV_PART_MAIN);
    lv_obj_set_style_radius(ui->device, 4, LV_PART_MAIN);
    lv_obj_set_style_transform_pivot_x(ui->device, 22, LV_PART_MAIN);
    lv_obj_set_style_transform_pivot_y(ui->device, 8, LV_PART_MAIN);

    ui->check = create_center_label(ui->normal, LV_SYMBOL_OK, 48,
                                    &lv_font_montserrat_32,
                                    ui_theme_color_accent());
    ui->angle_label = create_center_label(ui->normal, "0 / 35 deg", 136,
                                          &lv_font_montserrat_12,
                                          ui_theme_color_muted());

    ui->warning = lv_obj_create(ui->page);
    make_plain(ui->warning);
    lv_obj_set_size(ui->warning, 300, 150);
    lv_obj_align(ui->warning, LV_ALIGN_CENTER, 0, 10);
    create_center_label(ui->warning, "!", 14, &lv_font_montserrat_32,
                        ui_theme_color_accent());
    create_center_label(ui->warning, "Tilt out of range", 68,
                        &lv_font_montserrat_20, ui_theme_color_accent());
    create_center_label(ui->warning, "Adjust the stand - retrying...", 104,
                        &lv_font_montserrat_12, ui_theme_color_muted());
}

void ui_tilt_render(ui_tilt_t * ui, const app_state_t * state)
{
    char angle[32];
    bool failed;
    if(ui == NULL || state == NULL) return;
    failed = state->tilt_state == APP_CALIBRATION_FAILED;
    if(failed) {
        lv_obj_add_flag(ui->normal, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(ui->warning, LV_OBJ_FLAG_HIDDEN);
        return;
    }
    lv_obj_clear_flag(ui->normal, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->warning, LV_OBJ_FLAG_HIDDEN);
    lv_arc_set_value(ui->ring, state->tilt_progress);
    lv_obj_set_style_transform_rotation(ui->device,
        -state->tilt_measurement_deg * 10, LV_PART_MAIN);
    lv_snprintf(angle, sizeof(angle), "%d / %d deg",
                state->tilt_measurement_deg, state->tilt_target_deg);
    lv_label_set_text(ui->angle_label, angle);
    if(state->tilt_state == APP_CALIBRATION_PASSED) lv_obj_clear_flag(ui->check, LV_OBJ_FLAG_HIDDEN);
    else lv_obj_add_flag(ui->check, LV_OBJ_FLAG_HIDDEN);
}
