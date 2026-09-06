#include "ui/ui_app.h"

#include "app/app.h"
#include "lvgl/lvgl.h"
#include "ui/ui_bluetooth.h"
#include "ui/ui_brightness.h"
#include "ui/ui_countdown.h"
#include "ui/ui_diagram.h"
#include "ui/ui_home.h"
#include "ui/ui_mode_select.h"
#include "ui/ui_radar.h"
#include "ui/ui_result.h"
#include "ui/ui_theme.h"
#include "ui/ui_tilt.h"
#include "ui/ui_volume.h"
#include "ui/ui_workout.h"

static ui_home_t home;
static ui_brightness_t brightness;
static ui_volume_t volume;
static ui_bluetooth_t bluetooth;
static ui_mode_select_t mode_select;
static ui_diagram_t diagram;
static ui_tilt_t tilt;
static ui_radar_t radar;
static ui_countdown_t countdown;
static ui_workout_t workout;
static ui_result_t result;
static uint32_t last_app_tick;

static void set_page_visible(lv_obj_t * page, bool visible)
{
    if(visible) {
        lv_obj_clear_flag(page, LV_OBJ_FLAG_HIDDEN);
    }
    else {
        lv_obj_add_flag(page, LV_OBJ_FLAG_HIDDEN);
    }
}

static void app_tick_timer_cb(lv_timer_t * timer)
{
    (void)timer;

    uint32_t now = lv_tick_get();
    uint32_t elapsed_ms = now - last_app_tick;
    last_app_tick = now;
    if(app_tick(elapsed_ms)) {
        ui_app_render();
    }
}

void ui_app_init(void)
{
    lv_obj_t * screen = lv_screen_active();

    ui_theme_init();
    ui_theme_apply_screen(screen);
    ui_home_create(&home, screen);
    ui_brightness_create(&brightness, screen);
    ui_volume_create(&volume, screen);
    ui_bluetooth_create(&bluetooth, screen);
    ui_diagram_create(&diagram, screen);
    ui_tilt_create(&tilt, screen);
    ui_radar_create(&radar, screen);
    ui_countdown_create(&countdown, screen);
    ui_workout_create(&workout, screen);
    ui_result_create(&result, screen);
    ui_mode_select_create(&mode_select, screen);

    last_app_tick = lv_tick_get();
    lv_timer_create(app_tick_timer_cb, 40, NULL);
    ui_app_render();
}

void ui_app_render(void)
{
    const app_state_t * state = app_get_state();
    app_screen_t screen = state->screen;

    if(screen < APP_SCREEN_HOME || screen > APP_SCREEN_RESULT) {
        screen = APP_SCREEN_HOME;
    }

    set_page_visible(home.page, screen == APP_SCREEN_HOME ||
                                screen == APP_SCREEN_MODE_SELECT);
    set_page_visible(brightness.page, screen == APP_SCREEN_BRIGHTNESS);
    set_page_visible(volume.page, screen == APP_SCREEN_VOLUME);
    set_page_visible(bluetooth.page, screen == APP_SCREEN_BLUETOOTH);
    set_page_visible(mode_select.page, screen == APP_SCREEN_MODE_SELECT);
    set_page_visible(diagram.page, screen == APP_SCREEN_DIAGRAM);
    set_page_visible(tilt.page, screen == APP_SCREEN_TILT);
    set_page_visible(radar.page, screen == APP_SCREEN_RADAR);
    set_page_visible(countdown.page, screen == APP_SCREEN_COUNTDOWN);
    set_page_visible(workout.page, screen == APP_SCREEN_WORKOUT);
    set_page_visible(result.page, screen == APP_SCREEN_RESULT);

    if(screen == APP_SCREEN_HOME) {
        ui_home_render(&home, state);
    }
    else if(screen == APP_SCREEN_BRIGHTNESS) {
        ui_brightness_render(&brightness, state);
    }
    else if(screen == APP_SCREEN_VOLUME) {
        ui_volume_render(&volume, state);
    }
    else if(screen == APP_SCREEN_BLUETOOTH) {
        ui_bluetooth_render(&bluetooth, state);
    }
    else if(screen == APP_SCREEN_MODE_SELECT) {
        ui_mode_select_render(&mode_select, state);
    }
    else if(screen == APP_SCREEN_DIAGRAM) {
        ui_diagram_render(&diagram, state);
    }
    else if(screen == APP_SCREEN_TILT) {
        ui_tilt_render(&tilt, state);
    }
    else if(screen == APP_SCREEN_RADAR) {
        ui_radar_render(&radar, state);
    }
    else if(screen == APP_SCREEN_COUNTDOWN) {
        ui_countdown_render(&countdown, state);
    }
    else if(screen == APP_SCREEN_WORKOUT) {
        ui_workout_render(&workout, state);
    }
    else if(screen == APP_SCREEN_RESULT) {
        ui_result_render(&result, state);
    }
}
