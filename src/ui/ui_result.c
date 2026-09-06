#include "ui/ui_result.h"

#include <stddef.h>

#include "app/app.h"
#include "ui/ui_app.h"
#include "ui/ui_theme.h"

static void result_done_cb(lv_event_t * event)
{
    if(lv_event_get_code(event) == LV_EVENT_CLICKED) {
        app_dispatch(UI_ACTION_OK);
        ui_app_render();
    }
}

static void result_sync_cb(lv_event_t * event)
{
    if(lv_event_get_code(event) == LV_EVENT_CLICKED) {
        app_dispatch(UI_ACTION_SYNC);
        ui_app_render();
    }
}

static lv_obj_t * create_button(lv_obj_t * parent, const char * text,
                                bool primary, lv_event_cb_t callback)
{
    lv_obj_t * button = lv_button_create(parent);
    lv_obj_set_size(button, 112, 38);
    lv_obj_set_style_radius(button, 10, LV_PART_MAIN);
    lv_obj_set_style_bg_color(button, primary ? ui_theme_color_accent()
                                             : lv_color_hex(0x10251D), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(button, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_color(button, ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_border_width(button, 1, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(button, 0, LV_PART_MAIN);
    lv_obj_add_event_cb(button, callback, LV_EVENT_CLICKED, NULL);
    lv_obj_t * label = lv_label_create(button);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_color(label, primary ? ui_theme_color_card_ink()
                                              : ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_center(label);
    return button;
}

static lv_obj_t * create_stats_label(lv_obj_t * parent)
{
    lv_obj_t * label = lv_label_create(parent);
    lv_obj_set_style_text_color(label, ui_theme_color_muted(), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_12, LV_PART_MAIN);
    return label;
}

void ui_result_create(ui_result_t * ui, lv_obj_t * parent)
{
    if(ui == NULL || parent == NULL) return;
    ui->page = lv_obj_create(parent);
    ui_theme_apply_page(ui->page);
    lv_obj_set_size(ui->page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(ui->page, lv_color_hex(0x040609), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui->page, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_flex_flow(ui->page, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui->page, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t * sheet = lv_obj_create(ui->page);
    lv_obj_set_size(sheet, 286, 218);
    lv_obj_set_style_bg_color(sheet, lv_color_hex(0x0A0E0C), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(sheet, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_color(sheet, lv_color_hex(0x173328), LV_PART_MAIN);
    lv_obj_set_style_border_width(sheet, 1, LV_PART_MAIN);
    lv_obj_set_style_radius(sheet, 14, LV_PART_MAIN);
    lv_obj_set_style_pad_all(sheet, 10, LV_PART_MAIN);
    lv_obj_set_flex_flow(sheet, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(sheet, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(sheet, 7, LV_PART_MAIN);
    lv_obj_clear_flag(sheet, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * check = lv_label_create(sheet);
    lv_label_set_text(check, LV_SYMBOL_OK);
    lv_obj_set_style_text_color(check, ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_text_font(check, &lv_font_montserrat_32, LV_PART_MAIN);

    lv_obj_t * title = lv_label_create(sheet);
    lv_label_set_text(title, "Complete");
    lv_obj_set_style_text_color(title, ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, LV_PART_MAIN);

    ui->result_stats_row = lv_obj_create(sheet);
    lv_obj_remove_style_all(ui->result_stats_row);
    lv_obj_set_size(ui->result_stats_row, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(ui->result_stats_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui->result_stats_row, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(ui->result_stats_row, 8, LV_PART_MAIN);
    lv_obj_clear_flag(ui->result_stats_row,
                      LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);

    ui->reps_label = create_stats_label(ui->result_stats_row);

    lv_obj_t * separator_dot = lv_obj_create(ui->result_stats_row);
    lv_obj_remove_style_all(separator_dot);
    lv_obj_set_size(separator_dot, 4, 4);
    lv_obj_set_style_bg_color(separator_dot, ui_theme_color_muted(),
                              LV_PART_MAIN);
    lv_obj_set_style_bg_opa(separator_dot, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(separator_dot, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_clear_flag(separator_dot,
                      LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);

    ui->timer_label = create_stats_label(ui->result_stats_row);

    ui->plank_body = create_stats_label(sheet);
    lv_obj_set_width(ui->plank_body, LV_PCT(100));
    lv_obj_set_style_text_align(ui->plank_body, LV_TEXT_ALIGN_CENTER,
                                LV_PART_MAIN);
    lv_obj_add_flag(ui->plank_body, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t * actions = lv_obj_create(sheet);
    lv_obj_remove_style_all(actions);
    lv_obj_set_size(actions, 234, 38);
    lv_obj_set_flex_flow(actions, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(actions, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(actions, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    create_button(actions, "Sync", false, result_sync_cb);
    create_button(actions, "Done", true, result_done_cb);

    lv_obj_t * hint = lv_label_create(sheet);
    lv_label_set_text(hint, "Auto-home in 3s...");
    lv_obj_set_style_text_color(hint, ui_theme_color_muted_dark(), LV_PART_MAIN);
    lv_obj_set_style_text_font(hint, &lv_font_montserrat_12, LV_PART_MAIN);
}

void ui_result_render(ui_result_t * ui, const app_state_t * state)
{
    char text[48];
    uint32_t seconds;
    if(ui == NULL || state == NULL) return;
    seconds = state->result.elapsed_ms / 1000U;
    if(state->result.mode == APP_WORKOUT_MODE_PLANK) {
        lv_snprintf(text, sizeof(text), "Held %02u:%02u - nice core burn!",
                    (unsigned)(seconds / 60U), (unsigned)(seconds % 60U));
        lv_label_set_text(ui->plank_body, text);
        lv_obj_add_flag(ui->result_stats_row, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(ui->plank_body, LV_OBJ_FLAG_HIDDEN);
    }
    else {
        lv_snprintf(text, sizeof(text), "%u reps", state->result.reps);
        lv_label_set_text(ui->reps_label, text);
        lv_snprintf(text, sizeof(text), "%02u:%02u",
                    (unsigned)(seconds / 60U), (unsigned)(seconds % 60U));
        lv_label_set_text(ui->timer_label, text);
        lv_obj_clear_flag(ui->result_stats_row, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui->plank_body, LV_OBJ_FLAG_HIDDEN);
    }
}
