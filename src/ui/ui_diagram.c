#include "ui/ui_diagram.h"

#include <stddef.h>

#include "app/app.h"
#include "ui/ui_app.h"
#include "ui/ui_theme.h"

typedef struct {
    const char * distance;
    const char * height;
    int16_t target;
} diagram_values_t;

static const diagram_values_t values[APP_WORKOUT_MODE_COUNT] = {
    {"70-130 cm", "0 cm", 35}, {"70-130 cm", "0 cm", 35},
    {"70-90 cm", "1.2-1.5 m", 90}, {"70-130 cm", "0 cm", 35},
    {"0 cm", "0 cm", 0}, {"0 cm", "0 cm", 0}
};

static const lv_point_precise_t ground_points[] = {{18, 118}, {280, 118}};
static const lv_point_precise_t wall_points[] = {{270, 18}, {270, 119}};
static const lv_point_precise_t body_points[] = {{62, 47}, {62, 84}};
static const lv_point_precise_t arm_points[] = {{40, 65}, {62, 57}, {84, 65}};
static const lv_point_precise_t leg_left_points[] = {{62, 84}, {48, 112}};
static const lv_point_precise_t leg_right_points[] = {{62, 84}, {76, 112}};

static void make_plain(lv_obj_t * obj)
{
    lv_obj_remove_style_all(obj);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
}

static void create_line(lv_obj_t * parent, const lv_point_precise_t * points,
                        uint32_t count, lv_color_t color, int32_t width)
{
    lv_obj_t * line = lv_line_create(parent);
    make_plain(line);
    lv_line_set_points(line, points, count);
    lv_obj_set_style_line_color(line, color, LV_PART_MAIN);
    lv_obj_set_style_line_width(line, width, LV_PART_MAIN);
    lv_obj_set_style_line_rounded(line, true, LV_PART_MAIN);
}

static void skip_clicked_cb(lv_event_t * event)
{
    if(lv_event_get_code(event) == LV_EVENT_CLICKED) {
        app_dispatch(UI_ACTION_OK);
        ui_app_render();
    }
}

static lv_obj_t * create_value_chip(lv_obj_t * parent, int32_t x,
                                    const char * caption, lv_obj_t ** value)
{
    lv_obj_t * chip = lv_obj_create(parent);
    lv_obj_set_pos(chip, x, 161);
    lv_obj_set_size(chip, 144, 48);
    lv_obj_set_style_bg_color(chip, lv_color_hex(0x0E1814), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(chip, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_color(chip, lv_color_hex(0x173328), LV_PART_MAIN);
    lv_obj_set_style_border_width(chip, 1, LV_PART_MAIN);
    lv_obj_set_style_radius(chip, 9, LV_PART_MAIN);
    lv_obj_set_style_pad_all(chip, 3, LV_PART_MAIN);
    lv_obj_clear_flag(chip, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t * cap = lv_label_create(chip);
    lv_label_set_text(cap, caption);
    lv_obj_set_style_text_color(cap, ui_theme_color_muted(), LV_PART_MAIN);
    lv_obj_set_style_text_font(cap, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_align(cap, LV_ALIGN_TOP_MID, 0, 0);

    *value = lv_label_create(chip);
    lv_obj_set_style_text_color(*value, ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_text_font(*value, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_align(*value, LV_ALIGN_BOTTOM_MID, 0, 0);
    return chip;
}

void ui_diagram_create(ui_diagram_t * ui, lv_obj_t * parent)
{
    if(ui == NULL || parent == NULL) return;
    ui->page = lv_obj_create(parent);
    ui_theme_apply_page(ui->page);
    lv_obj_set_size(ui->page, LV_PCT(100), LV_PCT(100));

    lv_obj_t * box = lv_obj_create(ui->page);
    lv_obj_set_pos(box, 0, 0);
    lv_obj_set_size(box, 300, 146);
    lv_obj_set_style_bg_color(box, lv_color_hex(0x04060A), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(box, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_color(box, lv_color_hex(0x173328), LV_PART_MAIN);
    lv_obj_set_style_border_width(box, 1, LV_PART_MAIN);
    lv_obj_set_style_radius(box, 8, LV_PART_MAIN);
    lv_obj_clear_flag(box, LV_OBJ_FLAG_SCROLLABLE);

    create_line(box, ground_points, 2, lv_color_hex(0x173328), 2);
    create_line(box, wall_points, 2, lv_color_hex(0x173328), 2);
    create_line(box, body_points, 2, ui_theme_color_accent(), 3);
    create_line(box, arm_points, 3, ui_theme_color_accent(), 3);
    create_line(box, leg_left_points, 2, ui_theme_color_accent(), 3);
    create_line(box, leg_right_points, 2, ui_theme_color_accent(), 3);

    lv_obj_t * head = lv_obj_create(box);
    make_plain(head);
    lv_obj_set_size(head, 16, 16);
    lv_obj_set_pos(head, 54, 29);
    lv_obj_set_style_border_color(head, ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_border_width(head, 3, LV_PART_MAIN);
    lv_obj_set_style_radius(head, LV_RADIUS_CIRCLE, LV_PART_MAIN);

    ui->device = lv_obj_create(box);
    make_plain(ui->device);
    lv_obj_set_size(ui->device, 42, 14);
    lv_obj_set_pos(ui->device, 188, 96);
    lv_obj_set_style_border_color(ui->device, ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_border_width(ui->device, 3, LV_PART_MAIN);
    lv_obj_set_style_radius(ui->device, 4, LV_PART_MAIN);
    lv_obj_set_style_transform_pivot_x(ui->device, 21, LV_PART_MAIN);
    lv_obj_set_style_transform_pivot_y(ui->device, 7, LV_PART_MAIN);

    ui->target_label = lv_label_create(box);
    lv_obj_set_style_text_color(ui->target_label, ui_theme_color_muted(), LV_PART_MAIN);
    lv_obj_set_style_text_font(ui->target_label, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_align(ui->target_label, LV_ALIGN_TOP_RIGHT, -8, 8);

    lv_obj_t * skip = lv_button_create(ui->page);
    lv_obj_set_size(skip, 58, 28);
    lv_obj_align(skip, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_set_style_bg_opa(skip, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(skip, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(skip, 0, LV_PART_MAIN);
    lv_obj_add_event_cb(skip, skip_clicked_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * skip_label = lv_label_create(skip);
    lv_label_set_text(skip_label, "Skip >");
    lv_obj_set_style_text_color(skip_label, ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_text_font(skip_label, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_center(skip_label);

    create_value_chip(ui->page, 0, "DISTANCE", &ui->distance_label);
    create_value_chip(ui->page, 156, "HEIGHT", &ui->height_label);

    ui->progress = lv_bar_create(ui->page);
    lv_obj_set_pos(ui->progress, 0, 220);
    lv_obj_set_size(ui->progress, 300, 4);
    lv_bar_set_range(ui->progress, 0, 100);
    lv_obj_set_style_bg_color(ui->progress, lv_color_hex(0x1B2430), LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui->progress, ui_theme_color_accent(), LV_PART_INDICATOR);
    lv_obj_set_style_radius(ui->progress, 2, LV_PART_MAIN | LV_PART_INDICATOR);
}

void ui_diagram_render(ui_diagram_t * ui, const app_state_t * state)
{
    char target[24];
    app_workout_mode_t mode;
    if(ui == NULL || state == NULL) return;
    mode = state->workout_mode < APP_WORKOUT_MODE_COUNT ? state->workout_mode : APP_WORKOUT_MODE_JUMP;
    lv_label_set_text(ui->distance_label, values[mode].distance);
    lv_label_set_text(ui->height_label, values[mode].height);
    lv_snprintf(target, sizeof(target), "TARGET %d deg", values[mode].target);
    lv_label_set_text(ui->target_label, target);
    lv_obj_set_style_transform_rotation(ui->device, -values[mode].target * 10, LV_PART_MAIN);
    lv_bar_set_value(ui->progress, state->diagram_progress, LV_ANIM_OFF);
}
