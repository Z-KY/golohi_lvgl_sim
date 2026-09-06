#include "ui/ui_home.h"

#include "app/app.h"
#include "ui/ui_app.h"
#include "ui/ui_theme.h"

enum {
    /* The enabled 12 px LVGL font makes the HTML's 8 px chip about 4 px taller. */
    STATUSBAR_HEIGHT = 26,
    STATUSBAR_GAP = 5,
    BATTERY_WRAP_WIDTH = 21,
    BATTERY_WIDTH = 18,
    BATTERY_HEIGHT = 9,
    BATTERY_FILL_WIDTH = 11,
    CARD_WRAP_VERTICAL_PAD = 5,
    CARD_CONTENT_GAP = 16,
    CARD_ICON_SIZE = 104,
    DOTS_HEIGHT = 10,
    DOT_SIZE = 6,
    DOT_GAP = 7,
    DOT_COUNT = 4,
    BLUETOOTH_ICON_SIZE = 11
};

static const lv_point_precise_t icon_ground_points[] = {{13, 94}, {91, 94}};
static const lv_point_precise_t icon_body_points[] = {{52, 37}, {52, 54}};
static const lv_point_precise_t icon_left_arm_points[] = {{52, 44}, {36, 33}};
static const lv_point_precise_t icon_right_arm_points[] = {{52, 44}, {68, 33}};
static const lv_point_precise_t icon_left_leg_points[] = {{52, 54}, {42, 68}};
static const lv_point_precise_t icon_right_leg_points[] = {{52, 54}, {62, 68}};
static const lv_point_precise_t brightness_ray_top[] = {{52, 8}, {52, 25}};
static const lv_point_precise_t brightness_ray_bottom[] = {{52, 79}, {52, 96}};
static const lv_point_precise_t brightness_ray_left[] = {{8, 52}, {25, 52}};
static const lv_point_precise_t brightness_ray_right[] = {{79, 52}, {96, 52}};
static const lv_point_precise_t brightness_ray_top_left[] = {{21, 21}, {33, 33}};
static const lv_point_precise_t brightness_ray_top_right[] = {{71, 33}, {83, 21}};
static const lv_point_precise_t brightness_ray_bottom_left[] = {{21, 83}, {33, 71}};
static const lv_point_precise_t brightness_ray_bottom_right[] = {{71, 71}, {83, 83}};
static const lv_point_precise_t volume_speaker_points[] = {
    {18, 43}, {33, 43}, {50, 29}, {50, 75}, {33, 61}, {18, 61}, {18, 43}
};
static const lv_point_precise_t volume_wave_inner_points[] = {
    {59, 40}, {66, 47}, {66, 57}, {59, 64}
};
static const lv_point_precise_t volume_wave_outer_points[] = {
    {70, 31}, {80, 41}, {83, 52}, {80, 63}, {70, 73}
};
static const lv_point_precise_t card_bluetooth_points[] = {
    {43, 8}, {75, 35}, {28, 76}, {43, 95}, {43, 8}, {75, 68}, {28, 27}
};
static const lv_point_precise_t bluetooth_points[] = {
    {4, 0}, {8, 3}, {2, 8}, {4, 10}, {4, 0}, {8, 7}, {2, 2}
};

typedef struct {
    const char * title;
    bool is_workout;
} home_card_presentation_t;

static const home_card_presentation_t card_presentations[APP_HOME_CARD_COUNT] = {
    [APP_HOME_CARD_WORKOUT] = {"Jump", true},
    [APP_HOME_CARD_BRIGHTNESS] = {"BRIGHT", false},
    [APP_HOME_CARD_VOLUME] = {"VOLUME", false},
    [APP_HOME_CARD_BLUETOOTH] = {"BT", false}
};

static const char * const workout_mode_names[APP_WORKOUT_MODE_COUNT] = {
    [APP_WORKOUT_MODE_JUMP] = "Jump",
    [APP_WORKOUT_MODE_JUMPING_JACKS] = "Jumping Jacks",
    [APP_WORKOUT_MODE_BOXING] = "Boxing",
    [APP_WORKOUT_MODE_SQUATS] = "Squats",
    [APP_WORKOUT_MODE_PUSH_UPS] = "Push-ups",
    [APP_WORKOUT_MODE_PLANK] = "Plank"
};

static void big_card_pointer_event_cb(lv_event_t * event)
{
    ui_home_t * home = lv_event_get_user_data(event);
    lv_indev_t * indev = lv_event_get_indev(event);

    if(home == NULL || indev == NULL ||
       lv_indev_get_type(indev) != LV_INDEV_TYPE_POINTER) {
        return;
    }

    lv_event_code_t code = lv_event_get_code(event);
    if(code == LV_EVENT_PRESSED) {
        lv_indev_get_point(indev, &home->pointer_press_start);
        home->pointer_press_active = true;
        return;
    }

    if(code != LV_EVENT_RELEASED || !home->pointer_press_active) {
        return;
    }

    lv_point_t release_point;
    lv_indev_get_point(indev, &release_point);
    home->pointer_press_active = false;

    int32_t dx = release_point.x - home->pointer_press_start.x;
    int32_t dy = release_point.y - home->pointer_press_start.y;
    int32_t abs_dx = dx < 0 ? -dx : dx;
    int32_t abs_dy = dy < 0 ? -dy : dy;
    ui_action_t action = UI_ACTION_OK;

    if(abs_dx > 40 && abs_dx > abs_dy) {
        action = dx < 0 ? UI_ACTION_RIGHT : UI_ACTION_LEFT;
    }

    app_dispatch(action);
    ui_app_render();
}

static void make_plain(lv_obj_t * obj)
{
    lv_obj_remove_style_all(obj);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
}

static lv_obj_t * create_line(lv_obj_t * parent,
                              const lv_point_precise_t * points,
                              uint32_t point_count,
                              bool is_ground)
{
    lv_obj_t * line = lv_line_create(parent);
    lv_line_set_points(line, points, point_count);
    if(is_ground) {
        ui_theme_apply_icon_ground(line);
    }
    else {
        ui_theme_apply_icon_line(line);
    }
    lv_obj_set_pos(line, 0, 0);
    return line;
}

static lv_obj_t * create_accent_line(lv_obj_t * parent,
                                     const lv_point_precise_t * points,
                                     uint32_t point_count)
{
    lv_obj_t * line = lv_line_create(parent);
    lv_obj_remove_style_all(line);
    lv_line_set_points(line, points, point_count);
    lv_obj_set_style_line_color(line, ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_line_width(line, 5, LV_PART_MAIN);
    lv_obj_set_style_line_rounded(line, true, LV_PART_MAIN);
    lv_obj_set_pos(line, 0, 0);
    return line;
}

static lv_obj_t * create_bluetooth_icon(lv_obj_t * parent)
{
    lv_obj_t * icon = lv_obj_create(parent);
    make_plain(icon);
    lv_obj_set_size(icon, BLUETOOTH_ICON_SIZE, BLUETOOTH_ICON_SIZE);

    lv_obj_t * rune = lv_line_create(icon);
    lv_obj_remove_style_all(rune);
    lv_line_set_points(rune, bluetooth_points,
                       sizeof(bluetooth_points) / sizeof(bluetooth_points[0]));
    lv_obj_set_style_line_color(rune, ui_theme_color_muted(), LV_PART_MAIN);
    lv_obj_set_style_line_width(rune, 1, LV_PART_MAIN);
    lv_obj_set_style_line_rounded(rune, true, LV_PART_MAIN);
    lv_obj_set_pos(rune, 0, 0);
    return icon;
}

static lv_obj_t * create_battery(lv_obj_t * parent)
{
    lv_obj_t * battery = lv_obj_create(parent);
    make_plain(battery);
    lv_obj_set_size(battery, BATTERY_WRAP_WIDTH, BATTERY_HEIGHT);

    lv_obj_t * body = lv_obj_create(battery);
    make_plain(body);
    lv_obj_set_size(body, BATTERY_WIDTH, BATTERY_HEIGHT);
    lv_obj_set_style_border_color(body, ui_theme_color_muted(), LV_PART_MAIN);
    lv_obj_set_style_border_width(body, 1, LV_PART_MAIN);
    lv_obj_set_style_radius(body, 2, LV_PART_MAIN);

    lv_obj_t * fill = lv_obj_create(body);
    make_plain(fill);
    lv_obj_set_size(fill, BATTERY_FILL_WIDTH, 5);
    lv_obj_set_pos(fill, 2, 2);
    lv_obj_set_style_bg_color(fill, ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(fill, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(fill, 1, LV_PART_MAIN);

    lv_obj_t * terminal = lv_obj_create(battery);
    make_plain(terminal);
    lv_obj_set_size(terminal, 2, 4);
    lv_obj_set_pos(terminal, 19, 2);
    lv_obj_set_style_bg_color(terminal, ui_theme_color_muted(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(terminal, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(terminal, 1, LV_PART_MAIN);

    return battery;
}

static lv_obj_t * create_statusbar(lv_obj_t * parent)
{
    lv_obj_t * statusbar = lv_obj_create(parent);
    ui_theme_apply_statusbar(statusbar);
    lv_obj_set_size(statusbar, LV_PCT(100), STATUSBAR_HEIGHT);
    lv_obj_set_flex_flow(statusbar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(statusbar, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(statusbar, STATUSBAR_GAP, LV_PART_MAIN);

    lv_obj_t * brand = lv_label_create(statusbar);
    lv_label_set_text(brand, "GOLOHI");

    lv_obj_t * spacer = lv_obj_create(statusbar);
    make_plain(spacer);
    lv_obj_set_height(spacer, 1);
    lv_obj_set_flex_grow(spacer, 1);

    lv_obj_t * battery_wrap = lv_obj_create(statusbar);
    make_plain(battery_wrap);
    lv_obj_set_size(battery_wrap, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(battery_wrap, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(battery_wrap, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(battery_wrap, 4, LV_PART_MAIN);

    create_battery(battery_wrap);

    lv_obj_t * battery_text = lv_label_create(battery_wrap);
    lv_label_set_text(battery_text, "78%");
    lv_obj_set_style_text_color(battery_text, ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_text_font(battery_text, &lv_font_montserrat_12, LV_PART_MAIN);

    lv_obj_t * chip = lv_obj_create(statusbar);
    ui_theme_apply_chip(chip);
    lv_obj_set_size(chip, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(chip, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(chip, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(chip, 3, LV_PART_MAIN);
    lv_obj_clear_flag(chip, LV_OBJ_FLAG_CLICKABLE);

    create_bluetooth_icon(chip);

    lv_obj_t * bluetooth_text = lv_label_create(chip);
    lv_label_set_text(bluetooth_text, "BT");

    return statusbar;
}

static lv_obj_t * create_jump_icon(lv_obj_t * parent)
{
    lv_obj_t * icon = lv_obj_create(parent);
    make_plain(icon);
    lv_obj_set_size(icon, CARD_ICON_SIZE, CARD_ICON_SIZE);

    create_line(icon, icon_ground_points, 2, true);
    create_line(icon, icon_body_points, 2, false);
    create_line(icon, icon_left_arm_points, 2, false);
    create_line(icon, icon_right_arm_points, 2, false);
    create_line(icon, icon_left_leg_points, 2, false);
    create_line(icon, icon_right_leg_points, 2, false);

    lv_obj_t * head = lv_obj_create(icon);
    make_plain(head);
    lv_obj_set_size(head, 17, 17);
    lv_obj_set_pos(head, 44, 20);
    lv_obj_set_style_border_color(head, ui_theme_color_card_ink(), LV_PART_MAIN);
    lv_obj_set_style_border_width(head, 5, LV_PART_MAIN);
    lv_obj_set_style_radius(head, LV_RADIUS_CIRCLE, LV_PART_MAIN);

    return icon;
}

static lv_obj_t * create_brightness_card_icon(lv_obj_t * parent)
{
    lv_obj_t * icon = lv_obj_create(parent);
    make_plain(icon);
    lv_obj_set_size(icon, CARD_ICON_SIZE, CARD_ICON_SIZE);

    lv_obj_t * center = lv_obj_create(icon);
    make_plain(center);
    lv_obj_set_size(center, 34, 34);
    lv_obj_set_pos(center, 35, 35);
    lv_obj_set_style_border_color(center, ui_theme_color_accent(), LV_PART_MAIN);
    lv_obj_set_style_border_width(center, 5, LV_PART_MAIN);
    lv_obj_set_style_radius(center, LV_RADIUS_CIRCLE, LV_PART_MAIN);

    create_accent_line(icon, brightness_ray_top, 2);
    create_accent_line(icon, brightness_ray_bottom, 2);
    create_accent_line(icon, brightness_ray_left, 2);
    create_accent_line(icon, brightness_ray_right, 2);
    create_accent_line(icon, brightness_ray_top_left, 2);
    create_accent_line(icon, brightness_ray_top_right, 2);
    create_accent_line(icon, brightness_ray_bottom_left, 2);
    create_accent_line(icon, brightness_ray_bottom_right, 2);

    return icon;
}

static lv_obj_t * create_volume_card_icon(lv_obj_t * parent)
{
    lv_obj_t * icon = lv_obj_create(parent);
    make_plain(icon);
    lv_obj_set_size(icon, CARD_ICON_SIZE, CARD_ICON_SIZE);

    create_accent_line(icon, volume_speaker_points,
                       sizeof(volume_speaker_points) /
                           sizeof(volume_speaker_points[0]));
    create_accent_line(icon, volume_wave_inner_points,
                       sizeof(volume_wave_inner_points) /
                           sizeof(volume_wave_inner_points[0]));
    create_accent_line(icon, volume_wave_outer_points,
                       sizeof(volume_wave_outer_points) /
                           sizeof(volume_wave_outer_points[0]));

    return icon;
}

static lv_obj_t * create_bluetooth_card_icon(lv_obj_t * parent)
{
    lv_obj_t * icon = lv_obj_create(parent);
    make_plain(icon);
    lv_obj_set_size(icon, CARD_ICON_SIZE, CARD_ICON_SIZE);

    create_accent_line(icon, card_bluetooth_points,
                       sizeof(card_bluetooth_points) /
                           sizeof(card_bluetooth_points[0]));
    return icon;
}

static void create_big_card(ui_home_t * home, lv_obj_t * parent)
{
    lv_obj_t * card = lv_obj_create(parent);
    home->card = card;
    ui_theme_apply_mode_card(card);
    lv_obj_set_size(card, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(card, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(card, CARD_CONTENT_GAP, LV_PART_MAIN);
    lv_obj_add_flag(card, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK);
    lv_obj_add_event_cb(card, big_card_pointer_event_cb, LV_EVENT_PRESSED, home);
    lv_obj_add_event_cb(card, big_card_pointer_event_cb, LV_EVENT_RELEASED, home);

    home->card_icons[APP_HOME_CARD_WORKOUT] = create_jump_icon(card);
    home->card_icons[APP_HOME_CARD_BRIGHTNESS] = create_brightness_card_icon(card);
    home->card_icons[APP_HOME_CARD_VOLUME] = create_volume_card_icon(card);
    home->card_icons[APP_HOME_CARD_BLUETOOTH] = create_bluetooth_card_icon(card);

    home->card_name = lv_label_create(card);
    ui_theme_apply_card_name(home->card_name);
}

static void create_dots(ui_home_t * home, lv_obj_t * parent)
{
    lv_obj_t * dots = lv_obj_create(parent);
    make_plain(dots);
    lv_obj_set_size(dots, LV_PCT(100), DOTS_HEIGHT);
    lv_obj_set_flex_flow(dots, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(dots, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(dots, DOT_GAP, LV_PART_MAIN);
    lv_obj_set_style_pad_top(dots, 3, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(dots, 1, LV_PART_MAIN);

    for(uint32_t i = 0; i < DOT_COUNT; ++i) {
        lv_obj_t * dot = lv_obj_create(dots);
        home->dots[i] = dot;
        make_plain(dot);
        lv_obj_set_size(dot, DOT_SIZE, DOT_SIZE);
        lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, LV_PART_MAIN);
        lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, LV_PART_MAIN);
    }
}

void ui_home_create(ui_home_t * home, lv_obj_t * parent)
{
    if(home == NULL) {
        return;
    }

    home->pointer_press_active = false;

    lv_obj_t * page = lv_obj_create(parent);
    home->page = page;
    ui_theme_apply_page(page);
    lv_obj_set_size(page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(page, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(page, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);

    create_statusbar(page);

    lv_obj_t * card_wrap = lv_obj_create(page);
    make_plain(card_wrap);
    lv_obj_set_width(card_wrap, LV_PCT(100));
    lv_obj_set_flex_grow(card_wrap, 1);
    lv_obj_set_style_pad_top(card_wrap, CARD_WRAP_VERTICAL_PAD, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(card_wrap, CARD_WRAP_VERTICAL_PAD, LV_PART_MAIN);
    create_big_card(home, card_wrap);

    create_dots(home, page);
}

void ui_home_render(ui_home_t * home, const app_state_t * state)
{
    if(home == NULL || state == NULL) {
        return;
    }

    app_home_card_t card = state->home_card;
    if(card < APP_HOME_CARD_WORKOUT || card >= APP_HOME_CARD_COUNT) {
        card = APP_HOME_CARD_WORKOUT;
    }

    const home_card_presentation_t * presentation = &card_presentations[card];
    const char * title = presentation->title;
    if(card == APP_HOME_CARD_WORKOUT) {
        app_workout_mode_t mode = state->workout_mode;
        if(mode < APP_WORKOUT_MODE_JUMP || mode >= APP_WORKOUT_MODE_COUNT) {
            mode = APP_WORKOUT_MODE_JUMP;
        }
        title = workout_mode_names[mode];
    }
    lv_label_set_text(home->card_name, title);
    ui_theme_set_home_card_variant(home->card, home->card_name,
                                   presentation->is_workout);

    for(uint32_t i = 0; i < APP_HOME_CARD_COUNT; ++i) {
        if(i == (uint32_t)card) {
            lv_obj_clear_flag(home->card_icons[i], LV_OBJ_FLAG_HIDDEN);
        }
        else {
            lv_obj_add_flag(home->card_icons[i], LV_OBJ_FLAG_HIDDEN);
        }

        lv_obj_set_style_bg_color(home->dots[i],
                                  i == (uint32_t)card
                                      ? ui_theme_color_accent()
                                      : ui_theme_color_muted_dark(),
                                  LV_PART_MAIN);
    }
}
