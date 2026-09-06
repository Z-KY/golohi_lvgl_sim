#ifndef GOLOHI_APP_H
#define GOLOHI_APP_H

#include "app/app_state.h"

#ifdef __cplusplus
extern "C" {
#endif

void app_init(void);
const app_state_t * app_get_state(void);
void app_dispatch(ui_action_t action);
void app_set_brightness(uint8_t brightness);
void app_set_volume(uint8_t volume);
void app_bluetooth_primary(void);
bool app_tick(uint32_t elapsed_ms);
void app_set_workout_mode_selection(app_workout_mode_t mode);
void app_cancel_mode_select(void);
void app_submit_tilt_measurement(int16_t degrees);
void app_submit_radar_progress(uint8_t progress);
void app_workout_add_rep(void);
void app_workout_set_form_tip(app_form_tip_t form_tip);
void app_workout_stop(void);
void app_result_done(void);
void app_result_sync(void);
void app_notice_dismiss(void);
void app_device_boot(void);
void app_device_home(void);
void app_device_start_workout(void);
void app_device_set_battery(uint8_t percent);
void app_device_trigger_low_battery(void);
void app_device_set_charging(bool charging);
void app_device_sleep(void);
void app_device_wake(void);
void app_device_sensor_error(void);
void app_device_force_reboot(void);
void app_device_reset(void);
void app_device_tilt_fail(void);
void app_device_radar_fail(void);
void app_power_request(void);
void app_power_cancel(void);
void app_power_off(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* GOLOHI_APP_H */
