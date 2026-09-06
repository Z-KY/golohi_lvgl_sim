#include "ui/ui_radar.h"

#include <stddef.h>

#include "ui/ui_theme.h"

static const lv_point_precise_t body_points[] = {{60, 52}, {60, 77}};
static const lv_point_precise_t arm_points[] = {{42, 65}, {60, 59}, {78, 65}};
static const lv_point_precise_t leg_left_points[] = {{60, 77}, {48, 98}};
static const lv_point_precise_t leg_right_points[] = {{60, 77}, {72, 98}};

static void make_plain(lv_obj_t * obj)
{
    lv_obj_remove_style_all(obj);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
}

static lv_obj_t * create_label(lv_obj_t * parent, const char * text,
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

static void create_person_line(lv_obj_t * parent,
                               const lv_point_precise_t * points,
                               uint32_t count)
{
    lv_obj_t * line = lv_line_create(parent);
    make_plain(line);
    lv_line_set_points(line, points, count);
    lv_obj_set_style_line_color(line, ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_line_width(line, 3, LV_PART_MAIN);
    lv_obj_set_style_line_rounded(line, true, LV_PART_MAIN);
}

void ui_radar_create(ui_radar_t * ui, lv_obj_t * parent)
{
    if(ui == NULL || parent == NULL) return;
    ui->page = lv_obj_create(parent);
    ui_theme_apply_page(ui->page);
    lv_obj_set_size(ui->page, LV_PCT(100), LV_PCT(100));
    create_label(ui->page, "POSITION", 12, &lv_font_montserrat_12,
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

    for(int32_t diameter = 84; diameter >= 44; diameter -= 20) {
        lv_obj_t * circle = lv_obj_create(ui->normal);
        make_plain(circle);
        lv_obj_set_size(circle, diameter, diameter);
        lv_obj_align(circle, LV_ALIGN_TOP_MID, 0, (124 - diameter) / 2);
        lv_obj_set_style_border_color(circle, ui_theme_color_muted_dark(), LV_PART_MAIN);
        lv_obj_set_style_border_width(circle, 1, LV_PART_MAIN);
        lv_obj_set_style_radius(circle, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    }

    lv_obj_t * person = lv_obj_create(ui->normal);
    make_plain(person);
    lv_obj_set_size(person, 120, 120);
    lv_obj_align(person, LV_ALIGN_TOP_MID, 0, 2);
    create_person_line(person, body_points, 2);
    create_person_line(person, arm_points, 3);
    create_person_line(person, leg_left_points, 2);
    create_person_line(person, leg_right_points, 2);
    lv_obj_t * head = lv_obj_create(person);
    make_plain(head);
    lv_obj_set_size(head, 14, 14);
    lv_obj_set_pos(head, 53, 36);
    lv_obj_set_style_border_color(head, ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_border_width(head, 3, LV_PART_MAIN);
    lv_obj_set_style_radius(head, LV_RADIUS_CIRCLE, LV_PART_MAIN);

    ui->check = create_label(ui->normal, LV_SYMBOL_OK, 45,
                             &lv_font_montserrat_32, ui_theme_color_accent());
    create_label(ui->normal, "Step into the zone", 138,
                 &lv_font_montserrat_12, ui_theme_color_muted());

    ui->warning = lv_obj_create(ui->page);
    make_plain(ui->warning);
    lv_obj_set_size(ui->warning, 300, 150);
    lv_obj_align(ui->warning, LV_ALIGN_CENTER, 0, 10);
    create_label(ui->warning, "!", 14, &lv_font_montserrat_32,
                 ui_theme_color_accent());
    create_label(ui->warning, "Position not detected", 68,
                 &lv_font_montserrat_20, ui_theme_color_accent());
    create_label(ui->warning, "Step into the zone - retrying...", 104,
                 &lv_font_montserrat_12, ui_theme_color_muted());
}

void ui_radar_render(ui_radar_t * ui, const app_state_t * state)
{
    bool failed;
    if(ui == NULL || state == NULL) return;
    failed = state->radar_state == APP_CALIBRATION_FAILED;
    if(failed) {
        lv_obj_add_flag(ui->normal, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(ui->warning, LV_OBJ_FLAG_HIDDEN);
        return;
    }
    lv_obj_clear_flag(ui->normal, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->warning, LV_OBJ_FLAG_HIDDEN);
    lv_arc_set_value(ui->ring, state->radar_progress);
    if(state->radar_state == APP_CALIBRATION_PASSED) lv_obj_clear_flag(ui->check, LV_OBJ_FLAG_HIDDEN);
    else lv_obj_add_flag(ui->check, LV_OBJ_FLAG_HIDDEN);
}
