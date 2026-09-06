#include "ui/ui_volume.h"

#include <stdint.h>
#include <stddef.h>

#include "app/app.h"
#include "ui/ui_app.h"
#include "ui/ui_back_button.h"
#include "ui/ui_theme.h"

enum {
    SET_ICON_SIZE = 46,
    SET_BAR_WIDTH = 176,
    SET_BAR_HEIGHT = 18,
    SET_BAR_GAP = 5,
    SET_BUTTON_WIDTH = 58,
    SET_BUTTON_HEIGHT = 42
};

static const char * const volume_names[] = {"Mute", "Low", "Mid", "High"};
static const lv_point_precise_t speaker_points[] = {
    {2, 18}, {10, 18}, {21, 9}, {21, 37}, {10, 28}, {2, 28}, {2, 18}
};
static const lv_point_precise_t wave_inner_points[] = {
    {27, 16}, {32, 20}, {32, 26}, {27, 30}
};
static const lv_point_precise_t wave_outer_points[] = {
    {34, 11}, {40, 17}, {42, 23}, {40, 29}, {34, 35}
};

static void make_plain(lv_obj_t * obj)
{
    lv_obj_remove_style_all(obj);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
}

static void create_icon_line(lv_obj_t * parent,
                             const lv_point_precise_t * points,
                             uint32_t point_count)
{
    lv_obj_t * line = lv_line_create(parent);
    make_plain(line);
    lv_line_set_points(line, points, point_count);
    lv_obj_set_style_line_color(line, ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_line_width(line, 3, LV_PART_MAIN);
    lv_obj_set_style_line_rounded(line, true, LV_PART_MAIN);
}

static lv_obj_t * create_volume_icon(lv_obj_t * parent)
{
    lv_obj_t * icon = lv_obj_create(parent);
    make_plain(icon);
    lv_obj_set_size(icon, SET_ICON_SIZE, SET_ICON_SIZE);
    create_icon_line(icon, speaker_points,
                     sizeof(speaker_points) / sizeof(speaker_points[0]));
    create_icon_line(icon, wave_inner_points,
                     sizeof(wave_inner_points) / sizeof(wave_inner_points[0]));
    create_icon_line(icon, wave_outer_points,
                     sizeof(wave_outer_points) / sizeof(wave_outer_points[0]));
    return icon;
}

static void step_clicked_cb(lv_event_t * event)
{
    if(lv_event_get_code(event) != LV_EVENT_CLICKED) {
        return;
    }

    intptr_t step = (intptr_t)lv_event_get_user_data(event);
    app_dispatch(step < 0 ? UI_ACTION_DOWN : UI_ACTION_UP);
    ui_app_render();
}

static void bar_pointer_cb(lv_event_t * event)
{
    lv_event_code_t code = lv_event_get_code(event);
    if(code != LV_EVENT_PRESSED && code != LV_EVENT_PRESSING) {
        return;
    }

    lv_obj_t * bar = lv_event_get_target_obj(event);
    lv_indev_t * indev = lv_event_get_indev(event);
    if(indev == NULL || lv_indev_get_type(indev) != LV_INDEV_TYPE_POINTER) {
        return;
    }

    lv_area_t area;
    lv_point_t point;
    lv_obj_get_coords(bar, &area);
    lv_indev_get_point(indev, &point);

    int32_t width = lv_area_get_width(&area);
    int32_t offset = point.x - area.x1;
    if(offset < 0) {
        offset = 0;
    }
    else if(offset > width) {
        offset = width;
    }

    /* Exact HTML behavior: round(f * 3), then clamp to 1..3. */
    int32_t level = (offset * 3 + width / 2) / width;
    if(level < 1) {
        level = 1;
    }
    else if(level > 3) {
        level = 3;
    }

    app_set_volume((uint8_t)level);
    ui_app_render();
}

static lv_obj_t * create_step_button(lv_obj_t * parent, const char * text,
                                     intptr_t step)
{
    lv_obj_t * button = lv_button_create(parent);
    lv_obj_set_size(button, SET_BUTTON_WIDTH, SET_BUTTON_HEIGHT);
    lv_obj_set_style_radius(button, 11, LV_PART_MAIN);
    lv_obj_set_style_bg_color(button, lv_color_hex(0x10251D), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(button, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_color(button, ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_border_width(button, 1, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(button, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(button, 0, LV_PART_MAIN);
    lv_obj_add_event_cb(button, step_clicked_cb, LV_EVENT_CLICKED,
                        (void *)step);

    lv_obj_t * label = lv_label_create(button);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_color(label, ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_20, LV_PART_MAIN);
    lv_obj_center(label);
    return button;
}

void ui_volume_create(ui_volume_t * volume, lv_obj_t * parent)
{
    if(volume == NULL || parent == NULL) {
        return;
    }

    volume->page = lv_obj_create(parent);
    ui_theme_apply_page(volume->page);
    lv_obj_set_size(volume->page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(volume->page, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(volume->page, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(volume->page, 14, LV_PART_MAIN);

    ui_back_button_create(volume->page);

    create_volume_icon(volume->page);

    volume->value_label = lv_label_create(volume->page);
    lv_obj_set_style_text_color(volume->value_label,
                                ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_text_font(volume->value_label,
                               &lv_font_montserrat_20, LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(volume->value_label, 1, LV_PART_MAIN);

    lv_obj_t * bar = lv_obj_create(volume->page);
    make_plain(bar);
    lv_obj_set_size(bar, SET_BAR_WIDTH, SET_BAR_HEIGHT);
    lv_obj_set_flex_flow(bar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(bar, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(bar, SET_BAR_GAP, LV_PART_MAIN);
    lv_obj_add_flag(bar, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK);
    lv_obj_add_event_cb(bar, bar_pointer_cb, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(bar, bar_pointer_cb, LV_EVENT_PRESSING, NULL);

    for(uint32_t i = 0; i < 4U; ++i) {
        lv_obj_t * segment = lv_obj_create(bar);
        volume->segments[i] = segment;
        make_plain(segment);
        lv_obj_set_height(segment, SET_BAR_HEIGHT);
        lv_obj_set_flex_grow(segment, 1);
        lv_obj_set_style_radius(segment, 2, LV_PART_MAIN);
        lv_obj_set_style_border_width(segment, 1, LV_PART_MAIN);
    }

    lv_obj_t * step_row = lv_obj_create(volume->page);
    make_plain(step_row);
    lv_obj_set_size(step_row, LV_SIZE_CONTENT, SET_BUTTON_HEIGHT);
    lv_obj_set_flex_flow(step_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(step_row, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(step_row, 20, LV_PART_MAIN);
    create_step_button(step_row, "-", -1);
    create_step_button(step_row, "+", 1);
}

void ui_volume_render(ui_volume_t * volume, const app_state_t * state)
{
    if(volume == NULL || state == NULL) {
        return;
    }

    uint8_t level = state->volume > 3U ? 3U : state->volume;
    lv_label_set_text(volume->value_label, volume_names[level]);

    for(uint32_t i = 0; i < 4U; ++i) {
        bool active = i < level;
        lv_obj_set_style_bg_color(volume->segments[i],
                                  active ? ui_theme_color_accent()
                                         : lv_color_hex(0x163A2E),
                                  LV_PART_MAIN);
        lv_obj_set_style_bg_opa(volume->segments[i], LV_OPA_COVER,
                                LV_PART_MAIN);
        lv_obj_set_style_border_color(volume->segments[i],
                                      active ? ui_theme_color_accent()
                                             : lv_color_hex(0x173328),
                                      LV_PART_MAIN);
    }
}
