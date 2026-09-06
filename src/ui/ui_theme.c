#include "ui/ui_theme.h"

/* Colors are taken from spec/UI.html :root. */
#define GOLOHI_COLOR_BLACK       0x000000
#define GOLOHI_COLOR_PANEL       0x0A0E0C
#define GOLOHI_COLOR_LINE        0x173328
#define GOLOHI_COLOR_ACCENT      0x4EE6A6
#define GOLOHI_COLOR_MUTED       0x2A9E77
#define GOLOHI_COLOR_MUTED_DARK  0x1D4A3A
#define GOLOHI_COLOR_CARD_INK    0x04231A

static lv_style_t screen_style;
static lv_style_t page_style;
static lv_style_t statusbar_style;
static lv_style_t chip_style;
static lv_style_t mode_card_style;
static lv_style_t card_name_style;
static lv_style_t icon_line_style;
static lv_style_t icon_ground_style;
static bool initialized;

static void reset_and_add_style(lv_obj_t * obj, const lv_style_t * style)
{
    lv_obj_remove_style_all(obj);
    lv_obj_add_style(obj, style, LV_PART_MAIN);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
}

void ui_theme_init(void)
{
    if(initialized) {
        return;
    }

    lv_style_init(&screen_style);
    lv_style_set_bg_color(&screen_style, lv_color_hex(GOLOHI_COLOR_BLACK));
    lv_style_set_bg_opa(&screen_style, LV_OPA_COVER);
    lv_style_set_border_width(&screen_style, 0);
    lv_style_set_radius(&screen_style, 0);
    lv_style_set_pad_all(&screen_style, 0);

    lv_style_init(&page_style);
    lv_style_set_bg_color(&page_style, lv_color_hex(GOLOHI_COLOR_BLACK));
    lv_style_set_bg_opa(&page_style, LV_OPA_COVER);
    lv_style_set_border_width(&page_style, 0);
    lv_style_set_radius(&page_style, 0);
    lv_style_set_pad_all(&page_style, 10);

    lv_style_init(&statusbar_style);
    lv_style_set_bg_opa(&statusbar_style, LV_OPA_TRANSP);
    lv_style_set_border_color(&statusbar_style, lv_color_hex(GOLOHI_COLOR_LINE));
    lv_style_set_border_width(&statusbar_style, 1);
    lv_style_set_border_side(&statusbar_style, LV_BORDER_SIDE_BOTTOM);
    lv_style_set_radius(&statusbar_style, 0);
    lv_style_set_pad_left(&statusbar_style, 0);
    lv_style_set_pad_right(&statusbar_style, 0);
    lv_style_set_pad_top(&statusbar_style, 0);
    lv_style_set_pad_bottom(&statusbar_style, 5);
    lv_style_set_text_color(&statusbar_style, lv_color_hex(GOLOHI_COLOR_MUTED));
    lv_style_set_text_font(&statusbar_style, &lv_font_montserrat_12);

    lv_style_init(&chip_style);
    lv_style_set_bg_color(&chip_style, lv_color_hex(GOLOHI_COLOR_PANEL));
    lv_style_set_bg_opa(&chip_style, LV_OPA_COVER);
    lv_style_set_border_color(&chip_style, lv_color_hex(GOLOHI_COLOR_LINE));
    lv_style_set_border_width(&chip_style, 1);
    lv_style_set_radius(&chip_style, 14);
    lv_style_set_pad_left(&chip_style, 5);
    lv_style_set_pad_right(&chip_style, 5);
    lv_style_set_pad_top(&chip_style, 2);
    lv_style_set_pad_bottom(&chip_style, 2);
    lv_style_set_text_color(&chip_style, lv_color_hex(GOLOHI_COLOR_MUTED));
    lv_style_set_text_font(&chip_style, &lv_font_montserrat_12);

    lv_style_init(&mode_card_style);
    lv_style_set_bg_color(&mode_card_style, lv_color_hex(GOLOHI_COLOR_ACCENT));
    lv_style_set_bg_opa(&mode_card_style, LV_OPA_COVER);
    lv_style_set_border_width(&mode_card_style, 0);
    lv_style_set_radius(&mode_card_style, 18);
    lv_style_set_pad_all(&mode_card_style, 0);

    lv_style_init(&card_name_style);
    lv_style_set_text_color(&card_name_style, lv_color_hex(GOLOHI_COLOR_CARD_INK));
    lv_style_set_text_font(&card_name_style, &lv_font_montserrat_20);
    lv_style_set_text_letter_space(&card_name_style, 2);

    lv_style_init(&icon_line_style);
    lv_style_set_line_color(&icon_line_style, lv_color_hex(GOLOHI_COLOR_CARD_INK));
    lv_style_set_line_width(&icon_line_style, 5);
    lv_style_set_line_rounded(&icon_line_style, true);

    lv_style_init(&icon_ground_style);
    lv_style_set_line_color(&icon_ground_style, lv_color_hex(GOLOHI_COLOR_LINE));
    lv_style_set_line_width(&icon_ground_style, 3);
    lv_style_set_line_rounded(&icon_ground_style, true);

    initialized = true;
}

void ui_theme_apply_screen(lv_obj_t * obj)
{
    reset_and_add_style(obj, &screen_style);
}

void ui_theme_apply_page(lv_obj_t * obj)
{
    reset_and_add_style(obj, &page_style);
}

void ui_theme_apply_statusbar(lv_obj_t * obj)
{
    reset_and_add_style(obj, &statusbar_style);
}

void ui_theme_apply_chip(lv_obj_t * obj)
{
    reset_and_add_style(obj, &chip_style);
}

void ui_theme_apply_mode_card(lv_obj_t * obj)
{
    reset_and_add_style(obj, &mode_card_style);
}

void ui_theme_apply_card_name(lv_obj_t * obj)
{
    reset_and_add_style(obj, &card_name_style);
}

void ui_theme_set_home_card_variant(lv_obj_t * card, lv_obj_t * name,
                                    bool is_workout)
{
    lv_obj_set_style_bg_color(card,
                              lv_color_hex(is_workout ? GOLOHI_COLOR_ACCENT
                                                      : GOLOHI_COLOR_PANEL),
                              LV_PART_MAIN);
    lv_obj_set_style_border_color(card,
                                  lv_color_hex(is_workout ? GOLOHI_COLOR_ACCENT
                                                          : GOLOHI_COLOR_LINE),
                                  LV_PART_MAIN);
    lv_obj_set_style_border_width(card, is_workout ? 0 : 1, LV_PART_MAIN);
    lv_obj_set_style_text_color(name,
                                lv_color_hex(is_workout ? GOLOHI_COLOR_CARD_INK
                                                        : GOLOHI_COLOR_MUTED),
                                LV_PART_MAIN);
}

void ui_theme_apply_icon_line(lv_obj_t * obj)
{
    reset_and_add_style(obj, &icon_line_style);
}

void ui_theme_apply_icon_ground(lv_obj_t * obj)
{
    reset_and_add_style(obj, &icon_ground_style);
}

lv_color_t ui_theme_color_accent(void)
{
    return lv_color_hex(GOLOHI_COLOR_ACCENT);
}

lv_color_t ui_theme_color_muted(void)
{
    return lv_color_hex(GOLOHI_COLOR_MUTED);
}

lv_color_t ui_theme_color_muted_dark(void)
{
    return lv_color_hex(GOLOHI_COLOR_MUTED_DARK);
}

lv_color_t ui_theme_color_card_ink(void)
{
    return lv_color_hex(GOLOHI_COLOR_CARD_INK);
}
