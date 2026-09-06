#ifndef GOLOHI_APP_STATE_H
#define GOLOHI_APP_STATE_H

#include <stdbool.h>
#include <stdint.h>

#include "app/ui_action.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    APP_HOME_CARD_WORKOUT = 0,
    APP_HOME_CARD_BRIGHTNESS,
    APP_HOME_CARD_VOLUME,
    APP_HOME_CARD_BLUETOOTH,
    APP_HOME_CARD_COUNT
} app_home_card_t;

typedef enum {
    APP_SCREEN_HOME = 0,
    APP_SCREEN_BRIGHTNESS,
    APP_SCREEN_VOLUME,
    APP_SCREEN_BLUETOOTH,
    APP_SCREEN_MODE_SELECT,
    APP_SCREEN_DIAGRAM,
    APP_SCREEN_TILT,
    APP_SCREEN_RADAR,
    APP_SCREEN_COUNTDOWN,
    APP_SCREEN_WORKOUT,
    APP_SCREEN_RESULT,
    APP_SCREEN_BOOT,
    APP_SCREEN_SLEEP,
    APP_SCREEN_CHARGING,
    APP_SCREEN_POWERED_OFF
} app_screen_t;

typedef enum {
    APP_NOTICE_NONE = 0,
    APP_NOTICE_UPLOADED,
    APP_NOTICE_CHARGED,
    APP_NOTICE_LOW_BATTERY,
    APP_NOTICE_SENSOR_ERROR
} app_notice_t;

typedef enum {
    APP_BT_OFF = 0,
    APP_BT_PAIRING,
    APP_BT_ON,
    APP_BT_TRANSFER
} app_bt_state_t;

typedef enum {
    APP_WORKOUT_MODE_JUMP = 0,
    APP_WORKOUT_MODE_JUMPING_JACKS,
    APP_WORKOUT_MODE_BOXING,
    APP_WORKOUT_MODE_SQUATS,
    APP_WORKOUT_MODE_PUSH_UPS,
    APP_WORKOUT_MODE_PLANK,
    APP_WORKOUT_MODE_COUNT
} app_workout_mode_t;

typedef enum {
    APP_CALIBRATION_RUNNING = 0,
    APP_CALIBRATION_PASSED,
    APP_CALIBRATION_FAILED
} app_calibration_state_t;

typedef enum {
    APP_FORM_TIP_NONE = 0,
    APP_FORM_TIP_FULL_RANGE,
    APP_FORM_TIP_STRAIGHT_LINE
} app_form_tip_t;

typedef struct {
    uint32_t elapsed_ms;
    uint16_t reps;
    bool paused;
    app_workout_mode_t mode;
    app_form_tip_t form_tip;
    uint32_t form_tip_elapsed_ms;
} app_workout_session_t;

typedef struct {
    uint32_t elapsed_ms;
    uint16_t reps;
    app_workout_mode_t mode;
} app_workout_result_t;

typedef struct {
    app_screen_t screen;
    app_notice_t notice;
    bool power_confirmation;
    app_home_card_t home_card;
    uint8_t battery_percent;
    bool charging;
    uint8_t brightness;
    uint8_t volume;
    app_bt_state_t bluetooth;
    uint8_t bt_progress;
    uint32_t bt_elapsed_ms;
    app_workout_mode_t workout_mode;
    app_workout_mode_t workout_mode_selection;
    uint32_t diagram_elapsed_ms;
    uint8_t diagram_progress;
    int16_t tilt_target_deg;
    int16_t tilt_measurement_deg;
    bool tilt_measurement_valid;
    uint8_t tilt_progress;
    uint32_t tilt_elapsed_ms;
    uint32_t tilt_stable_ms;
    uint32_t calibration_phase_elapsed_ms;
    app_calibration_state_t tilt_state;
    uint8_t radar_progress;
    uint32_t radar_elapsed_ms;
    app_calibration_state_t radar_state;
    uint8_t countdown;
    uint32_t countdown_elapsed_ms;
    app_workout_session_t workout;
    app_workout_result_t result;
    uint32_t result_elapsed_ms;
    uint32_t boot_elapsed_ms;
    uint32_t boot_duration_ms;
    uint32_t charging_elapsed_ms;
    uint32_t charged_elapsed_ms;
    bool charged_notice_shown;
} app_state_t;

void app_state_init(app_state_t * state);
void app_state_dispatch(app_state_t * state, ui_action_t action);
void app_state_set_brightness(app_state_t * state, uint8_t brightness);
void app_state_set_volume(app_state_t * state, uint8_t volume);
void app_state_bluetooth_primary(app_state_t * state);
bool app_state_tick(app_state_t * state, uint32_t elapsed_ms);
void app_state_set_workout_mode_selection(app_state_t * state,
                                          app_workout_mode_t mode);
void app_state_cancel_mode_select(app_state_t * state);
void app_state_submit_tilt_measurement(app_state_t * state, int16_t degrees);
void app_state_submit_radar_progress(app_state_t * state, uint8_t progress);
void app_state_workout_add_rep(app_state_t * state);
void app_state_workout_set_form_tip(app_state_t * state,
                                    app_form_tip_t form_tip);
void app_state_workout_stop(app_state_t * state);
void app_state_result_done(app_state_t * state);
void app_state_result_sync(app_state_t * state);
void app_state_notice_dismiss(app_state_t * state);
void app_state_device_boot(app_state_t * state);
void app_state_device_home(app_state_t * state);
void app_state_device_start_workout(app_state_t * state);
void app_state_device_set_battery(app_state_t * state, uint8_t percent);
void app_state_device_trigger_low_battery(app_state_t * state);
void app_state_device_set_charging(app_state_t * state, bool charging);
void app_state_device_sleep(app_state_t * state);
void app_state_device_wake(app_state_t * state);
void app_state_device_sensor_error(app_state_t * state);
void app_state_device_force_reboot(app_state_t * state);
void app_state_device_reset(app_state_t * state);
void app_state_device_tilt_fail(app_state_t * state);
void app_state_device_radar_fail(app_state_t * state);
void app_state_power_request(app_state_t * state);
void app_state_power_cancel(app_state_t * state);
void app_state_power_off(app_state_t * state);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* GOLOHI_APP_STATE_H */
