#include "ui/ui_dialog.h"

#include <stddef.h>

#include "app/app.h"
#include "ui/ui_app.h"
#include "ui/ui_theme.h"

typedef struct {
    const char * icon;
    const char * title;
    const char * message;
    uint32_t color;
} notice_presentation_t;

static const notice_presentation_t notices[] = {
    [APP_NOTICE_NONE] = {"", "", "", 0x4EE6A6},
    [APP_NOTICE_UPLOADED] = {LV_SYMBOL_OK, "Uploaded",
        "Workout synced to your phone.", 0x4EE6A6},
    [APP_NOTICE_CHARGED] = {LV_SYMBOL_OK, "Charged 100%",
        "You may unplug now.", 0x4EE6A6},
    [APP_NOTICE_LOW_BATTERY] = {LV_SYMBOL_WARNING, "Low Battery",
        "Battery <10%. Workouts locked - charge soon.", 0xFFB14E},
    [APP_NOTICE_SENSOR_ERROR] = {LV_SYMBOL_WARNING, "Sensor Error",
        "LIS3DH / radar self-check failed. Hold o 10s or RESET pin.", 0xFF695E}
};

static void set_visible(lv_obj_t * obj, bool visible)
{
    if(visible) lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    else lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
}

static lv_obj_t * create_button(lv_obj_t * parent, const char * text,
                                lv_event_cb_t callback)
{
    lv_obj_t * button = lv_button_create(parent);
    lv_obj_set_height(button, 34);
    lv_obj_set_flex_grow(button, 1);
    lv_obj_set_style_radius(button, 10, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(button, 0, LV_PART_MAIN);
    lv_obj_add_event_cb(button, callback, LV_EVENT_CLICKED, NULL);
    lv_obj_t * label = lv_label_create(button);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_center(label);
    return button;
}

static void ok_clicked_cb(lv_event_t * event)
{
    if(lv_event_get_code(event) == LV_EVENT_CLICKED) {
        app_notice_dismiss();
        ui_app_render();
    }
}

static void cancel_clicked_cb(lv_event_t * event)
{
    if(lv_event_get_code(event) == LV_EVENT_CLICKED) {
        app_power_cancel();
        ui_app_render();
    }
}

static void power_off_clicked_cb(lv_event_t * event)
{
    if(lv_event_get_code(event) == LV_EVENT_CLICKED) {
        app_power_off();
        ui_app_render();
    }
}

void ui_dialog_create(ui_dialog_t * dialog, lv_obj_t * parent)
{
    if(dialog == NULL || parent == NULL) return;
    dialog->overlay = lv_obj_create(parent);
    lv_obj_remove_style_all(dialog->overlay);
    lv_obj_set_size(dialog->overlay, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(dialog->overlay, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(dialog->overlay, LV_OPA_70, LV_PART_MAIN);
    lv_obj_add_flag(dialog->overlay, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(dialog->overlay, LV_OBJ_FLAG_SCROLLABLE);

    dialog->sheet = lv_obj_create(dialog->overlay);
    lv_obj_set_size(dialog->sheet, 296, 174);
    lv_obj_center(dialog->sheet);
    lv_obj_set_style_bg_color(dialog->sheet, lv_color_hex(0x0B110E), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(dialog->sheet, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_color(dialog->sheet, lv_color_hex(0x234737), LV_PART_MAIN);
    lv_obj_set_style_border_width(dialog->sheet, 1, LV_PART_MAIN);
    lv_obj_set_style_radius(dialog->sheet, 14, LV_PART_MAIN);
    lv_obj_set_style_pad_all(dialog->sheet, 11, LV_PART_MAIN);
    lv_obj_set_style_pad_row(dialog->sheet, 6, LV_PART_MAIN);
    lv_obj_set_flex_flow(dialog->sheet, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(dialog->sheet, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(dialog->sheet, LV_OBJ_FLAG_SCROLLABLE);

    dialog->icon_circle = lv_obj_create(dialog->sheet);
    lv_obj_set_size(dialog->icon_circle, 43, 43);
    lv_obj_set_style_radius(dialog->icon_circle, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_border_width(dialog->icon_circle, 1, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(dialog->icon_circle, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_clear_flag(dialog->icon_circle, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    dialog->icon = lv_label_create(dialog->icon_circle);
    lv_obj_set_style_text_font(dialog->icon, &lv_font_montserrat_20, LV_PART_MAIN);
    lv_obj_center(dialog->icon);

    dialog->title = lv_label_create(dialog->sheet);
    lv_obj_set_style_text_font(dialog->title, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_set_style_text_color(dialog->title, lv_color_white(), LV_PART_MAIN);
    dialog->message = lv_label_create(dialog->sheet);
    lv_obj_set_width(dialog->message, 272);
    lv_label_set_long_mode(dialog->message, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_align(dialog->message, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_font(dialog->message, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_set_style_text_color(dialog->message, ui_theme_color_muted(), LV_PART_MAIN);

    lv_obj_t * buttons = lv_obj_create(dialog->sheet);
    lv_obj_remove_style_all(buttons);
    lv_obj_set_size(buttons, LV_PCT(100), 34);
    lv_obj_set_flex_flow(buttons, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(buttons, 8, LV_PART_MAIN);
    lv_obj_clear_flag(buttons, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    dialog->ok_button = create_button(buttons, "OK", ok_clicked_cb);
    dialog->cancel_button = create_button(buttons, "Cancel", cancel_clicked_cb);
    dialog->power_off_button = create_button(buttons, "Power Off", power_off_clicked_cb);

    lv_obj_set_style_bg_color(dialog->ok_button, ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_text_color(dialog->ok_button, lv_color_hex(0x04231A), LV_PART_MAIN);
    lv_obj_set_style_bg_color(dialog->cancel_button, lv_color_hex(0x17231E), LV_PART_MAIN);
    lv_obj_set_style_text_color(dialog->cancel_button, ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_bg_color(dialog->power_off_button, ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_text_color(dialog->power_off_button, lv_color_hex(0x04231A), LV_PART_MAIN);
}

void ui_dialog_render(ui_dialog_t * dialog, const app_state_t * state)
{
    if(dialog == NULL || state == NULL) return;
    bool power = state->power_confirmation;
    bool visible = power || state->notice != APP_NOTICE_NONE;
    set_visible(dialog->overlay, visible);
    if(!visible) return;

    const notice_presentation_t * p;
    notice_presentation_t power_p = {LV_SYMBOL_POWER, "Power",
        "Hold o 3s power off - hold o 10s force reboot - RESET pin.", 0xFF695E};
    p = power ? &power_p : &notices[state->notice];
    lv_color_t color = lv_color_hex(p->color);
    lv_label_set_text(dialog->icon, p->icon);
    lv_label_set_text(dialog->title, p->title);
    lv_label_set_text(dialog->message, p->message);
    lv_obj_set_style_text_color(dialog->icon, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_border_color(dialog->icon_circle, color, LV_PART_MAIN);
    lv_obj_set_style_bg_color(dialog->icon_circle, color, LV_PART_MAIN);
    set_visible(dialog->ok_button, !power);
    set_visible(dialog->cancel_button, power);
    set_visible(dialog->power_off_button, power);
    lv_obj_move_foreground(dialog->overlay);
}
