#include "ui/ui_bluetooth.h"

#include <stddef.h>

#include "app/app.h"
#include "ui/ui_app.h"
#include "ui/ui_back_button.h"
#include "ui/ui_theme.h"

enum {
    BT_RING_SIZE = 104,
    BT_ICON_SIZE = 58,
    BT_ACTION_WIDTH = 186,
    BT_ACTION_HEIGHT = 38
};

static const lv_point_precise_t bluetooth_points[] = {
    {25, 3}, {43, 19}, {15, 45}, {25, 55}, {25, 3}, {43, 39}, {15, 13}
};

static void make_plain(lv_obj_t * obj)
{
    lv_obj_remove_style_all(obj);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
}

static lv_obj_t * create_bluetooth_icon(lv_obj_t * parent)
{
    lv_obj_t * icon = lv_obj_create(parent);
    make_plain(icon);
    lv_obj_set_size(icon, BT_ICON_SIZE, BT_ICON_SIZE);

    lv_obj_t * rune = lv_line_create(icon);
    make_plain(rune);
    lv_line_set_points(rune, bluetooth_points,
                       sizeof(bluetooth_points) / sizeof(bluetooth_points[0]));
    lv_obj_set_style_line_color(rune, ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_line_width(rune, 4, LV_PART_MAIN);
    lv_obj_set_style_line_rounded(rune, true, LV_PART_MAIN);
    return icon;
}

static void action_clicked_cb(lv_event_t * event)
{
    if(lv_event_get_code(event) != LV_EVENT_CLICKED) {
        return;
    }

    app_bluetooth_primary();
    ui_app_render();
}

void ui_bluetooth_create(ui_bluetooth_t * bluetooth, lv_obj_t * parent)
{
    if(bluetooth == NULL || parent == NULL) {
        return;
    }

    bluetooth->page = lv_obj_create(parent);
    ui_theme_apply_page(bluetooth->page);
    lv_obj_set_size(bluetooth->page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(bluetooth->page, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(bluetooth->page, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(bluetooth->page, 8, LV_PART_MAIN);

    ui_back_button_create(bluetooth->page);

    lv_obj_t * title = lv_label_create(bluetooth->page);
    lv_label_set_text(title, "BLUETOOTH");
    lv_obj_set_style_text_color(title, ui_theme_color_muted(), LV_PART_MAIN);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(title, 2, LV_PART_MAIN);

    lv_obj_t * ring_wrap = lv_obj_create(bluetooth->page);
    make_plain(ring_wrap);
    lv_obj_set_size(ring_wrap, BT_RING_SIZE, BT_RING_SIZE);

    bluetooth->ring = lv_arc_create(ring_wrap);
    lv_obj_set_size(bluetooth->ring, BT_RING_SIZE, BT_RING_SIZE);
    lv_obj_center(bluetooth->ring);
    lv_arc_set_rotation(bluetooth->ring, 270);
    lv_arc_set_bg_angles(bluetooth->ring, 0, 360);
    lv_arc_set_range(bluetooth->ring, 0, 100);
    lv_obj_set_style_arc_width(bluetooth->ring, 6, LV_PART_MAIN);
    lv_obj_set_style_arc_color(bluetooth->ring, lv_color_hex(0x1B2430),
                               LV_PART_MAIN);
    lv_obj_set_style_arc_width(bluetooth->ring, 6, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(bluetooth->ring, ui_theme_color_accent(),
                               LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(bluetooth->ring, LV_OPA_TRANSP, LV_PART_KNOB);
    lv_obj_clear_flag(bluetooth->ring, LV_OBJ_FLAG_CLICKABLE);

    bluetooth->spinner = lv_spinner_create(ring_wrap);
    lv_obj_set_size(bluetooth->spinner, BT_RING_SIZE, BT_RING_SIZE);
    lv_obj_center(bluetooth->spinner);
    lv_spinner_set_anim_params(bluetooth->spinner, 1100, 108);
    lv_obj_set_style_arc_width(bluetooth->spinner, 6, LV_PART_MAIN);
    lv_obj_set_style_arc_color(bluetooth->spinner, lv_color_hex(0x1B2430),
                               LV_PART_MAIN);
    lv_obj_set_style_arc_width(bluetooth->spinner, 6, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(bluetooth->spinner, ui_theme_color_accent(),
                               LV_PART_INDICATOR);
    lv_obj_clear_flag(bluetooth->spinner, LV_OBJ_FLAG_CLICKABLE);

    bluetooth->icon = create_bluetooth_icon(ring_wrap);
    lv_obj_center(bluetooth->icon);

    bluetooth->state_label = lv_label_create(bluetooth->page);
    lv_obj_set_style_text_color(bluetooth->state_label,
                                ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_text_font(bluetooth->state_label,
                               &lv_font_montserrat_20, LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(bluetooth->state_label, 1,
                                       LV_PART_MAIN);

    bluetooth->action_button = lv_button_create(bluetooth->page);
    lv_obj_set_size(bluetooth->action_button, BT_ACTION_WIDTH,
                    BT_ACTION_HEIGHT);
    lv_obj_set_style_radius(bluetooth->action_button, 11, LV_PART_MAIN);
    lv_obj_set_style_bg_color(bluetooth->action_button,
                              lv_color_hex(0x10251D), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(bluetooth->action_button, LV_OPA_COVER,
                            LV_PART_MAIN);
    lv_obj_set_style_border_color(bluetooth->action_button,
                                  ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_border_width(bluetooth->action_button, 1, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(bluetooth->action_button, 0, LV_PART_MAIN);
    lv_obj_add_event_cb(bluetooth->action_button, action_clicked_cb,
                        LV_EVENT_CLICKED, NULL);

    bluetooth->action_label = lv_label_create(bluetooth->action_button);
    lv_obj_set_style_text_color(bluetooth->action_label,
                                ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_text_font(bluetooth->action_label,
                               &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(bluetooth->action_label, 1,
                                       LV_PART_MAIN);
    lv_obj_center(bluetooth->action_label);
}

void ui_bluetooth_render(ui_bluetooth_t * bluetooth,
                         const app_state_t * state)
{
    if(bluetooth == NULL || state == NULL) {
        return;
    }

    const char * state_text = "OFF";
    const char * action_text = "PAIR";
    bool disabled = false;
    bool pairing = false;
    bool off = false;
    int32_t progress = 0;

    switch(state->bluetooth) {
        case APP_BT_PAIRING:
            state_text = "CONNECTING...";
            action_text = "PAIRING...";
            disabled = true;
            pairing = true;
            break;
        case APP_BT_ON:
            state_text = "PAIRED";
            action_text = "UPLOAD";
            break;
        case APP_BT_TRANSFER:
            state_text = "UPLOADING...";
            action_text = "UPLOADING...";
            disabled = true;
            progress = state->bt_progress;
            break;
        case APP_BT_OFF:
        default:
            off = true;
            break;
    }

    lv_label_set_text(bluetooth->state_label, state_text);
    lv_label_set_text(bluetooth->action_label, action_text);
    lv_arc_set_value(bluetooth->ring, progress);

    if(pairing) {
        lv_obj_clear_flag(bluetooth->spinner, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(bluetooth->ring, LV_OBJ_FLAG_HIDDEN);
    }
    else {
        lv_obj_add_flag(bluetooth->spinner, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(bluetooth->ring, LV_OBJ_FLAG_HIDDEN);
    }

    lv_obj_set_style_opa(bluetooth->icon, off ? LV_OPA_30 : LV_OPA_COVER,
                         LV_PART_MAIN);
    lv_obj_set_style_text_color(bluetooth->state_label,
                                off ? ui_theme_color_muted_dark()
                                    : ui_theme_color_accent(),
                                LV_PART_MAIN);
    if(disabled) {
        lv_obj_add_state(bluetooth->action_button, LV_STATE_DISABLED);
    }
    else {
        lv_obj_remove_state(bluetooth->action_button, LV_STATE_DISABLED);
    }
}
