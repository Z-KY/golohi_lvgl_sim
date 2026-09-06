#include "ui/ui_brightness.h"

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

static const lv_point_precise_t ray_top[] = {{23, 0}, {23, 8}};
static const lv_point_precise_t ray_bottom[] = {{23, 38}, {23, 46}};
static const lv_point_precise_t ray_left[] = {{0, 23}, {8, 23}};
static const lv_point_precise_t ray_right[] = {{38, 23}, {46, 23}};
static const lv_point_precise_t ray_top_left[] = {{6, 6}, {12, 12}};
static const lv_point_precise_t ray_top_right[] = {{34, 12}, {40, 6}};
static const lv_point_precise_t ray_bottom_left[] = {{6, 40}, {12, 34}};
static const lv_point_precise_t ray_bottom_right[] = {{34, 34}, {40, 40}};

static void make_plain(lv_obj_t * obj)
{
    lv_obj_remove_style_all(obj);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
}

static void create_ray(lv_obj_t * parent,
                       const lv_point_precise_t * points)
{
    lv_obj_t * line = lv_line_create(parent);
    make_plain(line);
    lv_line_set_points(line, points, 2);
    lv_obj_set_style_line_color(line, ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_line_width(line, 3, LV_PART_MAIN);
    lv_obj_set_style_line_rounded(line, true, LV_PART_MAIN);
}

static lv_obj_t * create_brightness_icon(lv_obj_t * parent)
{
    lv_obj_t * icon = lv_obj_create(parent);
    make_plain(icon);
    lv_obj_set_size(icon, SET_ICON_SIZE, SET_ICON_SIZE);

    lv_obj_t * center = lv_obj_create(icon);
    make_plain(center);
    lv_obj_set_size(center, 18, 18);
    lv_obj_set_pos(center, 14, 14);
    lv_obj_set_style_border_color(center, ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_border_width(center, 3, LV_PART_MAIN);
    lv_obj_set_style_radius(center, LV_RADIUS_CIRCLE, LV_PART_MAIN);

    create_ray(icon, ray_top);
    create_ray(icon, ray_bottom);
    create_ray(icon, ray_left);
    create_ray(icon, ray_right);
    create_ray(icon, ray_top_left);
    create_ray(icon, ray_top_right);
    create_ray(icon, ray_bottom_left);
    create_ray(icon, ray_bottom_right);
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

    /* Matches bindSetTouch(): round(f * 4), then clamp to 1..4. */
    int32_t level = (offset * 4 + width / 2) / width;
    if(level < 1) {
        level = 1;
    }
    else if(level > 4) {
        level = 4;
    }

    app_set_brightness((uint8_t)level);
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

void ui_brightness_create(ui_brightness_t * brightness, lv_obj_t * parent)
{
    if(brightness == NULL || parent == NULL) {
        return;
    }

    brightness->page = lv_obj_create(parent);
    ui_theme_apply_page(brightness->page);
    lv_obj_set_size(brightness->page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(brightness->page, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(brightness->page, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(brightness->page, 14, LV_PART_MAIN);

    ui_back_button_create(brightness->page);

    create_brightness_icon(brightness->page);

    brightness->value_label = lv_label_create(brightness->page);
    lv_obj_set_style_text_color(brightness->value_label,
                                ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_text_font(brightness->value_label,
                               &lv_font_montserrat_20, LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(brightness->value_label, 1,
                                       LV_PART_MAIN);

    lv_obj_t * bar = lv_obj_create(brightness->page);
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
        brightness->segments[i] = segment;
        make_plain(segment);
        lv_obj_set_height(segment, SET_BAR_HEIGHT);
        lv_obj_set_flex_grow(segment, 1);
        lv_obj_set_style_radius(segment, 2, LV_PART_MAIN);
        lv_obj_set_style_border_width(segment, 1, LV_PART_MAIN);
    }

    lv_obj_t * step_row = lv_obj_create(brightness->page);
    make_plain(step_row);
    lv_obj_set_size(step_row, LV_SIZE_CONTENT, SET_BUTTON_HEIGHT);
    lv_obj_set_flex_flow(step_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(step_row, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(step_row, 20, LV_PART_MAIN);
    create_step_button(step_row, "-", -1);
    create_step_button(step_row, "+", 1);
}

void ui_brightness_render(ui_brightness_t * brightness,
                          const app_state_t * state)
{
    if(brightness == NULL || state == NULL) {
        return;
    }

    uint8_t level = state->brightness;
    if(level < 1U) {
        level = 1U;
    }
    else if(level > 4U) {
        level = 4U;
    }

    lv_label_set_text_fmt(brightness->value_label, "%u / 4",
                          (unsigned int)level);
    for(uint32_t i = 0; i < 4U; ++i) {
        bool active = i < level;
        lv_obj_set_style_bg_color(brightness->segments[i],
                                  active ? ui_theme_color_accent()
                                         : lv_color_hex(0x163A2E),
                                  LV_PART_MAIN);
        lv_obj_set_style_bg_opa(brightness->segments[i], LV_OPA_COVER,
                                LV_PART_MAIN);
        lv_obj_set_style_border_color(brightness->segments[i],
                                      active ? ui_theme_color_accent()
                                             : lv_color_hex(0x173328),
                                      LV_PART_MAIN);
    }
}
