#include "ui/ui_device_pages.h"

#include <stddef.h>

#include "app/app.h"
#include "ui/ui_app.h"
#include "ui/ui_theme.h"

static void make_page(lv_obj_t * page)
{
    ui_theme_apply_page(page);
    lv_obj_set_size(page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(page, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_pad_all(page, 0, LV_PART_MAIN);
    lv_obj_set_flex_flow(page, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(page, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
}

static lv_obj_t * make_label(lv_obj_t * parent, const char * text,
                             const lv_font_t * font, lv_color_t color)
{
    lv_obj_t * label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, font, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, color, LV_PART_MAIN);
    return label;
}

static void sleep_clicked_cb(lv_event_t * event)
{
    if(lv_event_get_code(event) == LV_EVENT_CLICKED) {
        app_device_wake();
        ui_app_render();
    }
}

void ui_device_pages_create(ui_device_pages_t * pages, lv_obj_t * parent)
{
    if(pages == NULL || parent == NULL) return;

    pages->boot_page = lv_obj_create(parent);
    make_page(pages->boot_page);
    lv_obj_set_style_pad_row(pages->boot_page, 11, LV_PART_MAIN);
    lv_obj_t * boot_logo = make_label(pages->boot_page, "GOLOHI",
                                      &lv_font_montserrat_20,
                                      ui_theme_color_accent());
    lv_obj_set_style_text_letter_space(boot_logo, 3, LV_PART_MAIN);
    pages->boot_bar = lv_bar_create(pages->boot_page);
    lv_obj_set_size(pages->boot_bar, 100, 3);
    lv_bar_set_range(pages->boot_bar, 0, 100);
    lv_obj_set_style_bg_color(pages->boot_bar, lv_color_hex(0x17231E),
                              LV_PART_MAIN);
    lv_obj_set_style_bg_color(pages->boot_bar, ui_theme_color_accent(),
                              LV_PART_INDICATOR);
    lv_obj_set_style_radius(pages->boot_bar, 3, LV_PART_MAIN);
    lv_obj_set_style_radius(pages->boot_bar, 3, LV_PART_INDICATOR);
    make_label(pages->boot_page, "INITIALIZING...", &lv_font_montserrat_12,
               ui_theme_color_muted());

    pages->sleep_page = lv_obj_create(parent);
    make_page(pages->sleep_page);
    lv_obj_add_flag(pages->sleep_page, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(pages->sleep_page, sleep_clicked_cb, LV_EVENT_CLICKED,
                        NULL);
    lv_obj_t * sleep = make_label(pages->sleep_page, "•  z  z  Z",
                                  &lv_font_montserrat_12,
                                  lv_color_hex(0x1D2733));
    lv_obj_set_style_text_letter_space(sleep, 2, LV_PART_MAIN);

    pages->charging_page = lv_obj_create(parent);
    make_page(pages->charging_page);
    lv_obj_set_style_pad_row(pages->charging_page, 12, LV_PART_MAIN);
    lv_obj_t * ring_wrap = lv_obj_create(pages->charging_page);
    lv_obj_remove_style_all(ring_wrap);
    lv_obj_set_size(ring_wrap, 104, 104);
    lv_obj_clear_flag(ring_wrap, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    pages->charging_ring = lv_arc_create(ring_wrap);
    lv_obj_set_size(pages->charging_ring, 104, 104);
    lv_obj_center(pages->charging_ring);
    lv_arc_set_rotation(pages->charging_ring, 270);
    lv_arc_set_bg_angles(pages->charging_ring, 0, 360);
    lv_arc_set_range(pages->charging_ring, 0, 100);
    lv_obj_set_style_arc_width(pages->charging_ring, 5, LV_PART_MAIN);
    lv_obj_set_style_arc_color(pages->charging_ring, lv_color_hex(0x17231E),
                               LV_PART_MAIN);
    lv_obj_set_style_arc_width(pages->charging_ring, 5, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(pages->charging_ring, ui_theme_color_accent(),
                               LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(pages->charging_ring, LV_OPA_TRANSP, LV_PART_KNOB);
    lv_obj_clear_flag(pages->charging_ring, LV_OBJ_FLAG_CLICKABLE);
    pages->charging_percent = make_label(ring_wrap, "82%",
                                         &lv_font_montserrat_20,
                                         ui_theme_color_accent());
    lv_obj_center(pages->charging_percent);
    lv_obj_t * charging = make_label(pages->charging_page, "CHARGING",
                                     &lv_font_montserrat_12,
                                     ui_theme_color_muted());
    lv_obj_set_style_text_letter_space(charging, 3, LV_PART_MAIN);

    pages->powered_off_page = lv_obj_create(parent);
    make_page(pages->powered_off_page);
}

void ui_device_pages_render(ui_device_pages_t * pages, const app_state_t * state)
{
    if(pages == NULL || state == NULL) return;
    uint32_t boot_progress = state->boot_duration_ms == 0U ? 0U
        : (state->boot_elapsed_ms * 100U) / state->boot_duration_ms;
    if(boot_progress > 100U) boot_progress = 100U;
    lv_bar_set_value(pages->boot_bar, (int32_t)boot_progress, LV_ANIM_OFF);

    char percent[8];
    lv_snprintf(percent, sizeof(percent), "%u%%",
                (unsigned int)state->battery_percent);
    lv_label_set_text(pages->charging_percent, percent);
    lv_arc_set_value(pages->charging_ring, state->battery_percent);
}
