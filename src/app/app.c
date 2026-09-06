#include "app/app.h"

static app_state_t app_state;

void app_init(void)
{
    app_state_init(&app_state);
}

const app_state_t * app_get_state(void)
{
    return &app_state;
}

void app_dispatch(ui_action_t action)
{
    app_state_dispatch(&app_state, action);
}

void app_set_brightness(uint8_t brightness)
{
    app_state_set_brightness(&app_state, brightness);
}

void app_set_volume(uint8_t volume)
{
    app_state_set_volume(&app_state, volume);
}

void app_bluetooth_primary(void)
{
    app_state_bluetooth_primary(&app_state);
}

bool app_tick(uint32_t elapsed_ms)
{
    return app_state_tick(&app_state, elapsed_ms);
}

void app_set_workout_mode_selection(app_workout_mode_t mode)
{
    app_state_set_workout_mode_selection(&app_state, mode);
}

void app_cancel_mode_select(void)
{
    app_state_cancel_mode_select(&app_state);
}

void app_submit_tilt_measurement(int16_t degrees)
{
    app_state_submit_tilt_measurement(&app_state, degrees);
}

void app_submit_radar_progress(uint8_t progress)
{
    app_state_submit_radar_progress(&app_state, progress);
}

void app_workout_add_rep(void)
{
    app_state_workout_add_rep(&app_state);
}

void app_workout_set_form_tip(app_form_tip_t form_tip)
{
    app_state_workout_set_form_tip(&app_state, form_tip);
}

void app_workout_stop(void)
{
    app_state_workout_stop(&app_state);
}

void app_result_done(void)
{
    app_state_result_done(&app_state);
}

void app_result_sync(void)
{
    app_state_result_sync(&app_state);
}

void app_notice_dismiss(void) { app_state_notice_dismiss(&app_state); }
void app_device_boot(void) { app_state_device_boot(&app_state); }
void app_device_home(void) { app_state_device_home(&app_state); }
void app_device_start_workout(void) { app_state_device_start_workout(&app_state); }
void app_device_set_battery(uint8_t percent) { app_state_device_set_battery(&app_state, percent); }
void app_device_trigger_low_battery(void) { app_state_device_trigger_low_battery(&app_state); }
void app_device_set_charging(bool charging) { app_state_device_set_charging(&app_state, charging); }
void app_device_sleep(void) { app_state_device_sleep(&app_state); }
void app_device_wake(void) { app_state_device_wake(&app_state); }
void app_device_sensor_error(void) { app_state_device_sensor_error(&app_state); }
void app_device_force_reboot(void) { app_state_device_force_reboot(&app_state); }
void app_device_reset(void) { app_state_device_reset(&app_state); }
void app_device_tilt_fail(void) { app_state_device_tilt_fail(&app_state); }
void app_device_radar_fail(void) { app_state_device_radar_fail(&app_state); }
void app_power_request(void) { app_state_power_request(&app_state); }
void app_power_cancel(void) { app_state_power_cancel(&app_state); }
void app_power_off(void) { app_state_power_off(&app_state); }
