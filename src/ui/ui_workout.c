#include "ui/ui_workout.h"

#include <stddef.h>

#include "app/app.h"
#include "ui/ui_app.h"
#include "ui/ui_theme.h"

static const char * const mode_names[APP_WORKOUT_MODE_COUNT] = {
    "JUMP", "JUMPING JACKS", "BOXING", "SQUATS", "PUSH-UPS", "PLANK"
};

static void style_action_button(lv_obj_t * button, bool pause)
{
    lv_obj_set_size(button, 138, 40);
    lv_obj_set_style_radius(button, 11, LV_PART_MAIN);
    lv_obj_set_style_bg_color(button, lv_color_hex(pause ? 0x10251D : 0x211012), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(button, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_color(button,
        pause ? ui_theme_color_accent() : lv_color_hex(0xE64E62), LV_PART_MAIN);
    lv_obj_set_style_border_width(button, 1, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(button, 0, LV_PART_MAIN);
}

static void pause_clicked_cb(lv_event_t * event)
{
    if(lv_event_get_code(event) == LV_EVENT_CLICKED) {
        app_dispatch(UI_ACTION_OK);
        ui_app_render();
    }
}

static void stop_clicked_cb(lv_event_t * event)
{
    if(lv_event_get_code(event) == LV_EVENT_CLICKED) {
        app_dispatch(UI_ACTION_STOP);
        ui_app_render();
    }
}

void ui_workout_create(ui_workout_t * ui, lv_obj_t * parent)
{
    if(ui == NULL || parent == NULL) return;
    ui->page = lv_obj_create(parent);
    ui_theme_apply_page(ui->page);
    lv_obj_set_size(ui->page, LV_PCT(100), LV_PCT(100));

    ui->mode_label = lv_label_create(ui->page);
    lv_obj_set_style_text_color(ui->mode_label, ui_theme_color_muted(), LV_PART_MAIN);
    lv_obj_set_style_text_font(ui->mode_label, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(ui->mode_label, 2, LV_PART_MAIN);
    lv_obj_align(ui->mode_label, LV_ALIGN_TOP_MID, 0, 1);

    ui->primary = lv_label_create(ui->page);
    lv_obj_set_style_text_color(ui->primary, ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_text_font(ui->primary, &lv_font_montserrat_32, LV_PART_MAIN);
    lv_obj_set_style_transform_zoom(ui->primary, 390, LV_PART_MAIN);
    lv_obj_align(ui->primary, LV_ALIGN_TOP_MID, 0, 42);

    ui->timer = lv_label_create(ui->page);
    lv_obj_set_style_text_color(ui->timer, ui_theme_color_muted(), LV_PART_MAIN);
    lv_obj_set_style_text_font(ui->timer, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_align(ui->timer, LV_ALIGN_TOP_MID, 0, 104);

    ui->tip = lv_label_create(ui->page);
    lv_obj_set_size(ui->tip, 270, 30);
    lv_label_set_long_mode(ui->tip, LV_LABEL_LONG_DOT);
    lv_obj_set_style_text_align(ui->tip, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui->tip, ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_text_font(ui->tip, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui->tip, lv_color_hex(0x10251D), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui->tip, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_color(ui->tip, ui_theme_color_muted_dark(), LV_PART_MAIN);
    lv_obj_set_style_border_width(ui->tip, 1, LV_PART_MAIN);
    lv_obj_set_style_radius(ui->tip, 9, LV_PART_MAIN);
    lv_obj_set_style_pad_top(ui->tip, 7, LV_PART_MAIN);
    lv_obj_align(ui->tip, LV_ALIGN_TOP_MID, 0, 130);

    lv_obj_t * pause = lv_button_create(ui->page);
    style_action_button(pause, true);
    lv_obj_align(pause, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_add_event_cb(pause, pause_clicked_cb, LV_EVENT_CLICKED, NULL);
    ui->pause_label = lv_label_create(pause);
    lv_obj_set_style_text_color(ui->pause_label, ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_text_font(ui->pause_label, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_center(ui->pause_label);

    lv_obj_t * stop = lv_button_create(ui->page);
    style_action_button(stop, false);
    lv_obj_align(stop, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_add_event_cb(stop, stop_clicked_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * stop_label = lv_label_create(stop);
    lv_label_set_text(stop_label, "STOP");
    lv_obj_set_style_text_color(stop_label, lv_color_hex(0xE64E62), LV_PART_MAIN);
    lv_obj_set_style_text_font(stop_label, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_center(stop_label);
}

void ui_workout_render(ui_workout_t * ui, const app_state_t * state)
{
    char primary[16];
    char timer[16];
    const char * tip = "";
    bool plank;
    app_workout_mode_t mode;
    uint32_t total_seconds;
    if(ui == NULL || state == NULL) return;
    mode = state->workout.mode < APP_WORKOUT_MODE_COUNT ? state->workout.mode : APP_WORKOUT_MODE_JUMP;
    plank = mode == APP_WORKOUT_MODE_PLANK;
    total_seconds = state->workout.elapsed_ms / 1000U;
    lv_label_set_text(ui->mode_label, mode_names[mode]);
    lv_snprintf(timer, sizeof(timer), "%02u:%02u",
                (unsigned)(total_seconds / 60U), (unsigned)(total_seconds % 60U));
    if(plank) {
        lv_label_set_text(ui->primary, timer);
        lv_label_set_text(ui->timer, "HOLD");
    }
    else {
        lv_snprintf(primary, sizeof(primary), "%u", state->workout.reps);
        lv_label_set_text(ui->primary, primary);
        lv_label_set_text(ui->timer, timer);
    }
    if(state->workout.form_tip == APP_FORM_TIP_FULL_RANGE) tip = "Keep form - full range";
    else if(state->workout.form_tip == APP_FORM_TIP_STRAIGHT_LINE) tip = "Straight line - no sagging";
    lv_label_set_text(ui->tip, tip);
    if(state->workout.form_tip == APP_FORM_TIP_NONE) lv_obj_add_flag(ui->tip, LV_OBJ_FLAG_HIDDEN);
    else lv_obj_clear_flag(ui->tip, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(ui->pause_label, state->workout.paused ? "RESUME" : "PAUSE");
}
