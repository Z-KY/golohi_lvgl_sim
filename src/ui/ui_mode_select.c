#include "ui/ui_mode_select.h"

#include <stdint.h>
#include <stddef.h>

#include "app/app.h"
#include "ui/ui_app.h"
#include "ui/ui_back_button.h"
#include "ui/ui_theme.h"

enum {
    SHEET_WIDTH = 296,
    SHEET_HEIGHT = 216,
    HEADER_HEIGHT = 30,
    BACK_BUTTON_SIZE = 30,
    MODE_ITEM_HEIGHT = 84,
    MODE_ICON_AREA_SIZE = 46,
    MODE_ICON_SIZE = 40
};

typedef struct {
    const char * name;
    const char * description;
    lv_point_precise_t head;
    lv_point_precise_t lines[5][2];
} mode_presentation_t;

static const mode_presentation_t mode_presentations[APP_WORKOUT_MODE_COUNT] = {
    [APP_WORKOUT_MODE_JUMP] = {
        "Jump", "Jump in place", {20, 8},
        {{{20, 14}, {20, 23}}, {{20, 17}, {12, 11}},
         {{20, 17}, {28, 11}}, {{20, 23}, {15, 31}},
         {{20, 23}, {25, 31}}}
    },
    [APP_WORKOUT_MODE_JUMPING_JACKS] = {
        "Jumping Jacks", "Star jumps", {20, 7},
        {{{20, 13}, {20, 25}}, {{20, 16}, {8, 7}},
         {{20, 16}, {32, 7}}, {{20, 25}, {10, 37}},
         {{20, 25}, {30, 37}}}
    },
    [APP_WORKOUT_MODE_BOXING] = {
        "Boxing", "Straight punches", {18, 9},
        {{{18, 15}, {18, 28}}, {{18, 18}, {9, 14}},
         {{18, 18}, {34, 16}}, {{18, 28}, {12, 37}},
         {{18, 28}, {25, 37}}}
    },
    [APP_WORKOUT_MODE_SQUATS] = {
        "Squats", "Bodyweight", {20, 12},
        {{{20, 18}, {20, 28}}, {{20, 21}, {9, 25}},
         {{20, 21}, {31, 25}}, {{20, 28}, {11, 36}},
         {{20, 28}, {29, 34}}}
    },
    [APP_WORKOUT_MODE_PUSH_UPS] = {
        "Push-ups", "On the floor", {7, 24},
        {{{11, 25}, {29, 25}}, {{8, 27}, {8, 36}},
         {{29, 25}, {36, 36}}, {{8, 36}, {15, 36}},
         {{29, 25}, {22, 25}}}
    },
    [APP_WORKOUT_MODE_PLANK] = {
        "Plank", "Hold the pose", {7, 26},
        {{{11, 27}, {30, 27}}, {{8, 29}, {12, 36}},
         {{12, 36}, {19, 36}}, {{30, 27}, {37, 36}},
         {{30, 27}, {23, 27}}}
    }
};

static const lv_point_precise_t ground_points[] = {{3, 38}, {37, 38}};

static void make_plain(lv_obj_t * obj)
{
    lv_obj_remove_style_all(obj);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
}

static lv_obj_t * create_pose_icon(lv_obj_t * parent,
                                   const mode_presentation_t * presentation)
{
    lv_obj_t * area = lv_obj_create(parent);
    make_plain(area);
    lv_obj_set_size(area, MODE_ICON_AREA_SIZE, MODE_ICON_AREA_SIZE);
    lv_obj_set_style_bg_color(area, lv_color_hex(0x12201A), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(area, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(area, 8, LV_PART_MAIN);

    lv_obj_t * icon = lv_obj_create(area);
    make_plain(icon);
    lv_obj_set_size(icon, MODE_ICON_SIZE, MODE_ICON_SIZE);
    lv_obj_center(icon);

    lv_obj_t * ground = lv_line_create(icon);
    make_plain(ground);
    lv_line_set_points(ground, ground_points, 2);
    lv_obj_set_style_line_color(ground, lv_color_hex(0x173328), LV_PART_MAIN);
    lv_obj_set_style_line_width(ground, 2, LV_PART_MAIN);

    for(uint32_t i = 0; i < 5U; ++i) {
        lv_obj_t * line = lv_line_create(icon);
        make_plain(line);
        lv_line_set_points(line, presentation->lines[i], 2);
        lv_obj_set_style_line_color(line, ui_theme_color_accent(), LV_PART_MAIN);
        lv_obj_set_style_line_width(line, 2, LV_PART_MAIN);
        lv_obj_set_style_line_rounded(line, true, LV_PART_MAIN);
    }

    lv_obj_t * head = lv_obj_create(icon);
    make_plain(head);
    lv_obj_set_size(head, 8, 8);
    lv_obj_set_pos(head, presentation->head.x - 4,
                   presentation->head.y - 4);
    lv_obj_set_style_border_color(head, ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_border_width(head, 2, LV_PART_MAIN);
    lv_obj_set_style_radius(head, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    return area;
}

static void mode_item_clicked_cb(lv_event_t * event)
{
    if(lv_event_get_code(event) != LV_EVENT_CLICKED) {
        return;
    }

    lv_event_stop_bubbling(event);
    app_workout_mode_t mode =
        (app_workout_mode_t)(intptr_t)lv_event_get_user_data(event);
    app_set_workout_mode_selection(mode);
    app_dispatch(UI_ACTION_OK);
    ui_app_render();
}

static void overlay_clicked_cb(lv_event_t * event)
{
    ui_mode_select_t * mode_select = lv_event_get_user_data(event);
    if(lv_event_get_code(event) != LV_EVENT_CLICKED || mode_select == NULL ||
       lv_event_get_target_obj(event) != mode_select->page) {
        return;
    }

    app_dispatch(UI_ACTION_BACK);
    ui_app_render();
}

static lv_obj_t * create_mode_item(ui_mode_select_t * mode_select,
                                   lv_obj_t * parent,
                                   app_workout_mode_t mode)
{
    const mode_presentation_t * presentation = &mode_presentations[mode];
    lv_obj_t * item = lv_obj_create(parent);
    mode_select->items[mode] = item;
    lv_obj_remove_style_all(item);
    lv_obj_set_height(item, MODE_ITEM_HEIGHT);
    lv_obj_set_style_bg_color(item, lv_color_hex(0x0E1410), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(item, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(item, 1, LV_PART_MAIN);
    lv_obj_set_style_border_color(item, lv_color_hex(0x0E1410), LV_PART_MAIN);
    lv_obj_set_style_radius(item, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_all(item, 4, LV_PART_MAIN);
    lv_obj_set_style_pad_row(item, 2, LV_PART_MAIN);
    lv_obj_set_flex_flow(item, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(item, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    lv_obj_add_flag(item, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(item, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(item, mode_item_clicked_cb, LV_EVENT_CLICKED,
                        (void *)(intptr_t)mode);

    create_pose_icon(item, presentation);

    lv_obj_t * name = lv_label_create(item);
    lv_label_set_text(name, presentation->name);
    lv_obj_set_style_text_color(name, ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_text_font(name, &lv_font_montserrat_12, LV_PART_MAIN);

    lv_obj_t * description = lv_label_create(item);
    lv_label_set_text(description, presentation->description);
    lv_obj_set_style_text_color(description, ui_theme_color_muted(),
                                LV_PART_MAIN);
    lv_obj_set_style_text_font(description, &lv_font_montserrat_12,
                               LV_PART_MAIN);
    return item;
}

void ui_mode_select_create(ui_mode_select_t * mode_select, lv_obj_t * parent)
{
    if(mode_select == NULL || parent == NULL) {
        return;
    }

    mode_select->page = lv_obj_create(parent);
    lv_obj_remove_style_all(mode_select->page);
    lv_obj_set_size(mode_select->page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(mode_select->page, lv_color_hex(0x040609),
                              LV_PART_MAIN);
    lv_obj_set_style_bg_opa(mode_select->page, LV_OPA_70, LV_PART_MAIN);
    lv_obj_set_style_pad_all(mode_select->page, 12, LV_PART_MAIN);
    lv_obj_set_flex_flow(mode_select->page, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(mode_select->page, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_add_flag(mode_select->page, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(mode_select->page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(mode_select->page, overlay_clicked_cb,
                        LV_EVENT_CLICKED, mode_select);

    lv_obj_t * sheet = lv_obj_create(mode_select->page);
    lv_obj_remove_style_all(sheet);
    lv_obj_set_size(sheet, SHEET_WIDTH, SHEET_HEIGHT);
    lv_obj_set_style_bg_color(sheet, lv_color_hex(0x0A0E0C), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(sheet, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_color(sheet, lv_color_hex(0x173328), LV_PART_MAIN);
    lv_obj_set_style_border_width(sheet, 1, LV_PART_MAIN);
    lv_obj_set_style_radius(sheet, 14, LV_PART_MAIN);
    lv_obj_set_style_pad_all(sheet, 11, LV_PART_MAIN);
    lv_obj_set_style_pad_row(sheet, 8, LV_PART_MAIN);
    lv_obj_set_flex_flow(sheet, LV_FLEX_FLOW_COLUMN);
    lv_obj_clear_flag(sheet, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t * header = lv_obj_create(sheet);
    make_plain(header);
    lv_obj_set_size(header, LV_PCT(100), HEADER_HEIGHT);

    lv_obj_t * back_button = ui_back_button_create(header);
    lv_obj_set_size(back_button, BACK_BUTTON_SIZE, BACK_BUTTON_SIZE);
    lv_obj_align(back_button, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_clear_flag(back_button, LV_OBJ_FLAG_FLOATING);
    lv_obj_clear_flag(back_button, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_set_style_bg_opa(back_button, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_bg_color(back_button, ui_theme_color_muted_dark(),
                              LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(back_button, LV_OPA_70,
                            LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_border_width(back_button, 0, LV_PART_MAIN);
    lv_obj_set_style_outline_width(back_button, 0,
                                   LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_outline_width(back_button, 0,
                                   LV_PART_MAIN | LV_STATE_FOCUS_KEY);
    lv_obj_set_style_radius(back_button, 7, LV_PART_MAIN);

    lv_obj_t * back_icon = lv_obj_get_child(back_button, 0);
    lv_obj_set_style_text_color(back_icon, ui_theme_color_muted(),
                                LV_PART_MAIN);
    lv_obj_set_style_text_font(back_icon, &lv_font_montserrat_14,
                               LV_PART_MAIN);

    lv_obj_t * title = lv_label_create(header);
    lv_label_set_text(title, "Workout Mode");
    lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_color(title, ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_center(title);

    static const int32_t column_dsc[] = {
        LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST
    };
    static const int32_t row_dsc[] = {
        MODE_ITEM_HEIGHT, MODE_ITEM_HEIGHT, MODE_ITEM_HEIGHT,
        LV_GRID_TEMPLATE_LAST
    };

    lv_obj_t * list = lv_obj_create(sheet);
    lv_obj_remove_style_all(list);
    lv_obj_set_width(list, LV_PCT(100));
    lv_obj_set_flex_grow(list, 1);
    lv_obj_set_grid_dsc_array(list, column_dsc, row_dsc);
    lv_obj_set_style_pad_column(list, 6, LV_PART_MAIN);
    lv_obj_set_style_pad_row(list, 6, LV_PART_MAIN);
    lv_obj_set_scroll_dir(list, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(list, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_add_flag(list, LV_OBJ_FLAG_SCROLLABLE);

    for(uint32_t i = 0; i < APP_WORKOUT_MODE_COUNT; ++i) {
        lv_obj_t * item = create_mode_item(mode_select, list,
            (app_workout_mode_t)i);
        lv_obj_set_grid_cell(item, LV_GRID_ALIGN_STRETCH, i % 2U, 1,
                             LV_GRID_ALIGN_START, i / 2U, 1);
    }
}

void ui_mode_select_render(ui_mode_select_t * mode_select,
                           const app_state_t * state)
{
    if(mode_select == NULL || state == NULL) {
        return;
    }

    app_workout_mode_t selected = state->workout_mode_selection;
    if(selected < APP_WORKOUT_MODE_JUMP || selected >= APP_WORKOUT_MODE_COUNT) {
        selected = APP_WORKOUT_MODE_JUMP;
    }

    for(uint32_t i = 0; i < APP_WORKOUT_MODE_COUNT; ++i) {
        bool is_selected = i == (uint32_t)selected;
        lv_obj_set_style_bg_color(mode_select->items[i],
                                  lv_color_hex(is_selected ? 0x13211C
                                                           : 0x0E1410),
                                  LV_PART_MAIN);
        lv_obj_set_style_border_color(mode_select->items[i],
                                      is_selected
                                          ? ui_theme_color_accent()
                                          : lv_color_hex(0x0E1410),
                                      LV_PART_MAIN);
    }
}
