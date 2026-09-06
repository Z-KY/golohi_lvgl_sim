#include "ui/ui_back_button.h"

#include "app/app.h"
#include "ui/ui_app.h"
#include "ui/ui_theme.h"

enum {
    BACK_BUTTON_SIZE = 30,
    BACK_BUTTON_OFFSET = 8
};

static void back_clicked_cb(lv_event_t * event)
{
    (void)event;

    app_dispatch(UI_ACTION_BACK);
    ui_app_render();
}

lv_obj_t * ui_back_button_create(lv_obj_t * parent)
{
    if(parent == NULL) {
        return NULL;
    }

    lv_obj_t * button = lv_button_create(parent);
    lv_obj_set_size(button, BACK_BUTTON_SIZE, BACK_BUTTON_SIZE);
    lv_obj_align(button, LV_ALIGN_TOP_LEFT, BACK_BUTTON_OFFSET,
                 BACK_BUTTON_OFFSET);
    lv_obj_add_flag(button, LV_OBJ_FLAG_FLOATING);
    lv_obj_clear_flag(button, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(button, 7, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(button, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(button, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(button, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(button, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(button, ui_theme_color_muted_dark(),
                              LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(button, LV_OPA_70,
                            LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_event_cb(button, back_clicked_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * label = lv_label_create(button);
    lv_label_set_text(label, LV_SYMBOL_LEFT);
    lv_obj_set_style_text_color(label, ui_theme_color_muted(), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_center(label);
    return button;
}
