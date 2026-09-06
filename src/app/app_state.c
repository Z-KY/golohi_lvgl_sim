#include "app/app_state.h"

#include <stddef.h>

enum {
    BT_PAIRING_DURATION_MS = 1600,
    BT_TRANSFER_DURATION_MS = 1400,
    DIAGRAM_DURATION_MS = 10000,
    CALIBRATION_TIMEOUT_MS = 60000,
    CALIBRATION_STABLE_MS = 2000,
    CALIBRATION_PASS_DELAY_MS = 600,
    CALIBRATION_RETRY_DELAY_MS = 3000,
    FORM_TIP_DURATION_MS = 2600,
    RESULT_DURATION_MS = 3000,
    BOOT_DURATION_MS = 2000,
    FORCE_REBOOT_DURATION_MS = 1600,
    CHARGE_STEP_MS = 60,
    CHARGED_NOTICE_DELAY_MS = 700
};

static bool mode_is_valid(app_workout_mode_t mode)
{
    return mode >= APP_WORKOUT_MODE_JUMP && mode < APP_WORKOUT_MODE_COUNT;
}

static int16_t target_for_mode(app_workout_mode_t mode)
{
    if(mode == APP_WORKOUT_MODE_BOXING) return 90;
    if(mode == APP_WORKOUT_MODE_PUSH_UPS || mode == APP_WORKOUT_MODE_PLANK) return 0;
    return 35;
}

static void enter_home(app_state_t * state)
{
    state->screen = APP_SCREEN_HOME;
    state->result_elapsed_ms = 0;
}

static void enter_diagram(app_state_t * state)
{
    state->screen = APP_SCREEN_DIAGRAM;
    state->diagram_elapsed_ms = 0;
    state->diagram_progress = 0;
    state->tilt_target_deg = target_for_mode(state->workout_mode);
}

static void enter_tilt(app_state_t * state)
{
    state->screen = APP_SCREEN_TILT;
    state->tilt_target_deg = target_for_mode(state->workout_mode);
    state->tilt_measurement_deg = 0;
    state->tilt_measurement_valid = false;
    state->tilt_progress = 0;
    state->tilt_elapsed_ms = 0;
    state->tilt_stable_ms = 0;
    state->calibration_phase_elapsed_ms = 0;
    state->tilt_state = APP_CALIBRATION_RUNNING;
}

static void enter_radar(app_state_t * state)
{
    state->screen = APP_SCREEN_RADAR;
    state->radar_progress = 0;
    state->radar_elapsed_ms = 0;
    state->calibration_phase_elapsed_ms = 0;
    state->radar_state = APP_CALIBRATION_RUNNING;
}

static void enter_countdown(app_state_t * state)
{
    state->screen = APP_SCREEN_COUNTDOWN;
    state->countdown = 3;
    state->countdown_elapsed_ms = 0;
}

static void enter_workout(app_state_t * state)
{
    state->screen = APP_SCREEN_WORKOUT;
    state->workout.elapsed_ms = 0;
    state->workout.reps = 0;
    state->workout.paused = false;
    state->workout.mode = state->workout_mode;
    state->workout.form_tip = APP_FORM_TIP_NONE;
    state->workout.form_tip_elapsed_ms = 0;
}

void app_state_init(app_state_t * state)
{
    if(state == NULL) return;
    *state = (app_state_t){0};
    state->screen = APP_SCREEN_BOOT;
    state->boot_duration_ms = BOOT_DURATION_MS;
    state->home_card = APP_HOME_CARD_WORKOUT;
    state->battery_percent = 78;
    state->brightness = 3;
    state->volume = 2;
    state->bluetooth = APP_BT_OFF;
    state->workout_mode = APP_WORKOUT_MODE_JUMP;
    state->workout_mode_selection = APP_WORKOUT_MODE_JUMP;
    state->workout.mode = APP_WORKOUT_MODE_JUMP;
    state->result.mode = APP_WORKOUT_MODE_JUMP;
}

void app_state_set_brightness(app_state_t * state, uint8_t brightness)
{
    if(state == NULL) return;
    if(brightness < 1U) brightness = 1U;
    else if(brightness > 4U) brightness = 4U;
    state->brightness = brightness;
}

void app_state_set_volume(app_state_t * state, uint8_t volume)
{
    if(state != NULL) state->volume = volume > 3U ? 3U : volume;
}

void app_state_bluetooth_primary(app_state_t * state)
{
    if(state == NULL) return;
    if(state->bluetooth == APP_BT_OFF) {
        state->bluetooth = APP_BT_PAIRING;
        state->bt_progress = 0;
        state->bt_elapsed_ms = 0;
    }
    else if(state->bluetooth == APP_BT_ON) {
        state->bluetooth = APP_BT_TRANSFER;
        state->bt_progress = 0;
        state->bt_elapsed_ms = 0;
    }
}

static bool tick_bluetooth(app_state_t * state, uint32_t elapsed_ms)
{
    uint32_t duration;
    if(state->bluetooth == APP_BT_PAIRING) duration = BT_PAIRING_DURATION_MS;
    else if(state->bluetooth == APP_BT_TRANSFER) duration = BT_TRANSFER_DURATION_MS;
    else return false;

    uint8_t old_progress = state->bt_progress;
    uint32_t remaining = duration - state->bt_elapsed_ms;
    if(elapsed_ms >= remaining) {
        bool was_transfer = state->bluetooth == APP_BT_TRANSFER;
        state->bluetooth = APP_BT_ON;
        state->bt_progress = was_transfer ? 100U : 0U;
        state->bt_elapsed_ms = 0;
        if(was_transfer) state->notice = APP_NOTICE_UPLOADED;
        return true;
    }
    state->bt_elapsed_ms += elapsed_ms;
    if(state->bluetooth == APP_BT_TRANSFER) {
        state->bt_progress = (uint8_t)((state->bt_elapsed_ms * 100U) / duration);
    }
    return old_progress != state->bt_progress;
}

static bool tick_diagram(app_state_t * state, uint32_t elapsed_ms)
{
    uint8_t old_progress = state->diagram_progress;
    uint32_t remaining = DIAGRAM_DURATION_MS - state->diagram_elapsed_ms;
    if(elapsed_ms >= remaining) {
        enter_tilt(state);
        return true;
    }
    state->diagram_elapsed_ms += elapsed_ms;
    state->diagram_progress = (uint8_t)((state->diagram_elapsed_ms * 100U) /
                                        DIAGRAM_DURATION_MS);
    return old_progress != state->diagram_progress;
}

static bool tick_tilt(app_state_t * state, uint32_t elapsed_ms)
{
    if(state->tilt_state == APP_CALIBRATION_PASSED) {
        state->calibration_phase_elapsed_ms += elapsed_ms;
        if(state->calibration_phase_elapsed_ms >= CALIBRATION_PASS_DELAY_MS) {
            enter_radar(state);
            return true;
        }
        return false;
    }
    if(state->tilt_state == APP_CALIBRATION_FAILED) {
        state->calibration_phase_elapsed_ms += elapsed_ms;
        if(state->calibration_phase_elapsed_ms >= CALIBRATION_RETRY_DELAY_MS) {
            enter_tilt(state);
            return true;
        }
        return false;
    }

    state->tilt_elapsed_ms += elapsed_ms;
    if(state->tilt_measurement_valid) {
        int32_t error = state->tilt_measurement_deg - state->tilt_target_deg;
        if(error < 0) error = -error;
        if(error <= 15) state->tilt_stable_ms += elapsed_ms;
        else state->tilt_stable_ms = 0;
    }
    else state->tilt_stable_ms = 0;

    if(state->tilt_stable_ms >= CALIBRATION_STABLE_MS) {
        state->tilt_state = APP_CALIBRATION_PASSED;
        state->tilt_progress = 100;
        state->calibration_phase_elapsed_ms = 0;
        return true;
    }
    if(state->tilt_elapsed_ms >= CALIBRATION_TIMEOUT_MS) {
        state->tilt_state = APP_CALIBRATION_FAILED;
        state->calibration_phase_elapsed_ms = 0;
        state->tilt_stable_ms = 0;
        return true;
    }
    return false;
}

static bool tick_radar(app_state_t * state, uint32_t elapsed_ms)
{
    if(state->radar_state == APP_CALIBRATION_PASSED) {
        state->calibration_phase_elapsed_ms += elapsed_ms;
        if(state->calibration_phase_elapsed_ms >= CALIBRATION_PASS_DELAY_MS) {
            enter_countdown(state);
            return true;
        }
        return false;
    }
    if(state->radar_state == APP_CALIBRATION_FAILED) {
        state->calibration_phase_elapsed_ms += elapsed_ms;
        if(state->calibration_phase_elapsed_ms >= CALIBRATION_RETRY_DELAY_MS) {
            enter_radar(state);
            return true;
        }
        return false;
    }

    state->radar_elapsed_ms += elapsed_ms;
    if(state->radar_progress >= 100U) {
        state->radar_state = APP_CALIBRATION_PASSED;
        state->calibration_phase_elapsed_ms = 0;
        return true;
    }
    if(state->radar_elapsed_ms >= CALIBRATION_TIMEOUT_MS) {
        state->radar_state = APP_CALIBRATION_FAILED;
        state->calibration_phase_elapsed_ms = 0;
        return true;
    }
    return false;
}

static bool tick_countdown(app_state_t * state, uint32_t elapsed_ms)
{
    bool changed = false;
    state->countdown_elapsed_ms += elapsed_ms;
    while(state->countdown_elapsed_ms >= 1000U) {
        state->countdown_elapsed_ms -= 1000U;
        if(state->countdown > 1U) {
            --state->countdown;
            changed = true;
        }
        else {
            enter_workout(state);
            return true;
        }
    }
    return changed;
}

static bool tick_workout(app_state_t * state, uint32_t elapsed_ms)
{
    uint32_t old_second = state->workout.elapsed_ms / 1000U;
    if(!state->workout.paused) state->workout.elapsed_ms += elapsed_ms;
    if(state->workout.form_tip != APP_FORM_TIP_NONE) {
        state->workout.form_tip_elapsed_ms += elapsed_ms;
        if(state->workout.form_tip_elapsed_ms >= FORM_TIP_DURATION_MS) {
            state->workout.form_tip = APP_FORM_TIP_NONE;
            state->workout.form_tip_elapsed_ms = 0;
            return true;
        }
    }
    return old_second != state->workout.elapsed_ms / 1000U;
}

bool app_state_tick(app_state_t * state, uint32_t elapsed_ms)
{
    bool changed;
    if(state == NULL || elapsed_ms == 0U) return false;
    if(state->screen == APP_SCREEN_BOOT) {
        if(elapsed_ms >= state->boot_duration_ms - state->boot_elapsed_ms) {
            state->boot_elapsed_ms = state->boot_duration_ms;
            enter_home(state);
            return true;
        }
        state->boot_elapsed_ms += elapsed_ms;
        return true;
    }
    if(state->screen == APP_SCREEN_POWERED_OFF || state->screen == APP_SCREEN_SLEEP) {
        return false;
    }
    changed = tick_bluetooth(state, elapsed_ms);
    if(state->screen == APP_SCREEN_CHARGING && state->charging) {
        if(state->battery_percent < 100U) {
            uint8_t old_percent = state->battery_percent;
            state->charging_elapsed_ms += elapsed_ms;
            while(state->charging_elapsed_ms >= CHARGE_STEP_MS &&
                  state->battery_percent < 100U) {
                state->charging_elapsed_ms -= CHARGE_STEP_MS;
                ++state->battery_percent;
            }
            if(state->battery_percent == 100U) state->charged_elapsed_ms = 0;
            changed = changed || old_percent != state->battery_percent;
        }
        else if(!state->charged_notice_shown) {
            state->charged_elapsed_ms += elapsed_ms;
            if(state->charged_elapsed_ms >= CHARGED_NOTICE_DELAY_MS) {
                state->notice = APP_NOTICE_CHARGED;
                state->charged_notice_shown = true;
                changed = true;
            }
        }
        return changed;
    }
    switch(state->screen) {
        case APP_SCREEN_DIAGRAM: return tick_diagram(state, elapsed_ms) || changed;
        case APP_SCREEN_TILT: return tick_tilt(state, elapsed_ms) || changed;
        case APP_SCREEN_RADAR: return tick_radar(state, elapsed_ms) || changed;
        case APP_SCREEN_COUNTDOWN: return tick_countdown(state, elapsed_ms) || changed;
        case APP_SCREEN_WORKOUT: return tick_workout(state, elapsed_ms) || changed;
        case APP_SCREEN_RESULT:
            state->result_elapsed_ms += elapsed_ms;
            if(state->result_elapsed_ms >= RESULT_DURATION_MS) {
                enter_home(state);
                return true;
            }
            return changed;
        default: return changed;
    }
}

void app_state_set_workout_mode_selection(app_state_t * state,
                                          app_workout_mode_t mode)
{
    if(state != NULL && state->screen == APP_SCREEN_MODE_SELECT && mode_is_valid(mode)) {
        state->workout_mode_selection = mode;
    }
}

void app_state_cancel_mode_select(app_state_t * state)
{
    if(state != NULL && state->screen == APP_SCREEN_MODE_SELECT) {
        state->workout_mode_selection = state->workout_mode;
        state->screen = APP_SCREEN_HOME;
    }
}

void app_state_submit_tilt_measurement(app_state_t * state, int16_t degrees)
{
    int32_t error;
    if(state == NULL || state->screen != APP_SCREEN_TILT ||
       state->tilt_state != APP_CALIBRATION_RUNNING) return;
    if(degrees < 0) degrees = 0;
    else if(degrees > 90) degrees = 90;
    state->tilt_measurement_deg = degrees;
    state->tilt_measurement_valid = true;
    error = degrees - state->tilt_target_deg;
    if(error < 0) error = -error;
    state->tilt_progress = (uint8_t)(error >= 90 ? 0 : 100 - (error * 100 / 90));
}

void app_state_submit_radar_progress(app_state_t * state, uint8_t progress)
{
    if(state != NULL && state->screen == APP_SCREEN_RADAR &&
       state->radar_state == APP_CALIBRATION_RUNNING) {
        state->radar_progress = progress > 100U ? 100U : progress;
    }
}

void app_state_workout_add_rep(app_state_t * state)
{
    if(state != NULL && state->screen == APP_SCREEN_WORKOUT &&
       !state->workout.paused && state->workout.mode != APP_WORKOUT_MODE_PLANK &&
       state->workout.reps < UINT16_MAX) ++state->workout.reps;
}

void app_state_workout_set_form_tip(app_state_t * state,
                                    app_form_tip_t form_tip)
{
    if(state != NULL && state->screen == APP_SCREEN_WORKOUT &&
       form_tip >= APP_FORM_TIP_NONE && form_tip <= APP_FORM_TIP_STRAIGHT_LINE) {
        state->workout.form_tip = form_tip;
        state->workout.form_tip_elapsed_ms = 0;
    }
}

void app_state_workout_stop(app_state_t * state)
{
    if(state == NULL || state->screen != APP_SCREEN_WORKOUT) return;
    state->result.elapsed_ms = state->workout.elapsed_ms;
    state->result.reps = state->workout.reps;
    state->result.mode = state->workout.mode;
    state->result_elapsed_ms = 0;
    state->screen = APP_SCREEN_RESULT;
}

void app_state_result_done(app_state_t * state)
{
    if(state != NULL && state->screen == APP_SCREEN_RESULT) enter_home(state);
}

void app_state_result_sync(app_state_t * state)
{
    if(state == NULL || state->screen != APP_SCREEN_RESULT) return;
    state->screen = APP_SCREEN_BLUETOOTH;
    state->result_elapsed_ms = 0;
    app_state_bluetooth_primary(state);
}

static void clear_transient_state(app_state_t * state)
{
    state->notice = APP_NOTICE_NONE;
    state->power_confirmation = false;
    state->bt_elapsed_ms = 0;
    state->diagram_elapsed_ms = 0;
    state->diagram_progress = 0;
    state->tilt_elapsed_ms = 0;
    state->tilt_stable_ms = 0;
    state->radar_elapsed_ms = 0;
    state->calibration_phase_elapsed_ms = 0;
    state->countdown_elapsed_ms = 0;
    state->result_elapsed_ms = 0;
    state->charging_elapsed_ms = 0;
    state->charged_elapsed_ms = 0;
    state->charged_notice_shown = false;
    state->workout = (app_workout_session_t){0};
    state->workout.mode = state->workout_mode;
    if(state->bluetooth == APP_BT_PAIRING) state->bluetooth = APP_BT_OFF;
    else if(state->bluetooth == APP_BT_TRANSFER) state->bluetooth = APP_BT_ON;
}

void app_state_notice_dismiss(app_state_t * state)
{
    if(state == NULL || state->notice == APP_NOTICE_NONE) return;
    state->notice = APP_NOTICE_NONE;
    if(state->screen != APP_SCREEN_HOME && state->screen != APP_SCREEN_CHARGING)
        enter_home(state);
}

void app_state_device_boot(app_state_t * state)
{
    if(state == NULL) return;
    clear_transient_state(state);
    state->charging = false;
    state->boot_elapsed_ms = 0;
    state->boot_duration_ms = BOOT_DURATION_MS;
    state->screen = APP_SCREEN_BOOT;
}

void app_state_device_home(app_state_t * state)
{
    if(state == NULL || state->screen == APP_SCREEN_POWERED_OFF) return;
    state->notice = APP_NOTICE_NONE;
    state->power_confirmation = false;
    enter_home(state);
}

void app_state_device_start_workout(app_state_t * state)
{
    if(state == NULL || state->screen == APP_SCREEN_POWERED_OFF ||
       state->charging || state->battery_percent < 10U) return;
    state->notice = APP_NOTICE_NONE;
    state->power_confirmation = false;
    enter_diagram(state);
}

void app_state_device_set_battery(app_state_t * state, uint8_t percent)
{
    if(state == NULL) return;
    state->battery_percent = percent > 100U ? 100U : percent;
}

void app_state_device_trigger_low_battery(app_state_t * state)
{
    if(state == NULL) return;
    app_state_device_set_battery(state, 8U);
    if(!state->charging) {
        state->power_confirmation = false;
        state->notice = APP_NOTICE_LOW_BATTERY;
    }
}

void app_state_device_set_charging(app_state_t * state, bool charging)
{
    if(state == NULL) return;
    state->notice = APP_NOTICE_NONE;
    state->power_confirmation = false;
    state->charging = charging;
    state->charging_elapsed_ms = 0;
    state->charged_elapsed_ms = 0;
    state->charged_notice_shown = false;
    if(charging) {
        if(state->battery_percent < 82U) state->battery_percent = 82U;
        state->screen = APP_SCREEN_CHARGING;
    }
    else {
        enter_home(state);
    }
}

void app_state_device_sleep(app_state_t * state)
{
    if(state == NULL || state->screen == APP_SCREEN_WORKOUT ||
       state->screen == APP_SCREEN_CHARGING ||
       state->screen == APP_SCREEN_POWERED_OFF) return;
    state->notice = APP_NOTICE_NONE;
    state->power_confirmation = false;
    state->screen = APP_SCREEN_SLEEP;
}

void app_state_device_wake(app_state_t * state)
{
    if(state != NULL && state->screen == APP_SCREEN_SLEEP) enter_home(state);
}

void app_state_device_sensor_error(app_state_t * state)
{
    if(state == NULL || state->screen == APP_SCREEN_POWERED_OFF) return;
    state->bluetooth = APP_BT_OFF;
    state->bt_progress = 0;
    state->bt_elapsed_ms = 0;
    state->power_confirmation = false;
    state->notice = APP_NOTICE_SENSOR_ERROR;
}

void app_state_device_force_reboot(app_state_t * state)
{
    if(state == NULL) return;
    clear_transient_state(state);
    state->charging = false;
    state->boot_elapsed_ms = 0;
    state->boot_duration_ms = FORCE_REBOOT_DURATION_MS;
    state->screen = APP_SCREEN_BOOT;
}

void app_state_device_reset(app_state_t * state)
{
    if(state == NULL) return;
    app_state_init(state);
    state->screen = APP_SCREEN_HOME;
    state->boot_elapsed_ms = 0;
}

void app_state_device_tilt_fail(app_state_t * state)
{
    if(state == NULL || state->screen == APP_SCREEN_POWERED_OFF) return;
    enter_tilt(state);
    state->tilt_state = APP_CALIBRATION_FAILED;
}

void app_state_device_radar_fail(app_state_t * state)
{
    if(state == NULL || state->screen == APP_SCREEN_POWERED_OFF) return;
    enter_radar(state);
    state->radar_state = APP_CALIBRATION_FAILED;
}

void app_state_power_request(app_state_t * state)
{
    if(state == NULL || state->screen == APP_SCREEN_POWERED_OFF ||
       state->screen == APP_SCREEN_BOOT) return;
    state->notice = APP_NOTICE_NONE;
    state->power_confirmation = true;
}

void app_state_power_cancel(app_state_t * state)
{
    if(state != NULL) state->power_confirmation = false;
}

void app_state_power_off(app_state_t * state)
{
    if(state == NULL || !state->power_confirmation) return;
    clear_transient_state(state);
    state->charging = false;
    state->screen = APP_SCREEN_POWERED_OFF;
}

void app_state_dispatch(app_state_t * state, ui_action_t action)
{
    if(state == NULL) return;
    if(state->screen < APP_SCREEN_HOME || state->screen > APP_SCREEN_POWERED_OFF) state->screen = APP_SCREEN_HOME;
    if(state->screen == APP_SCREEN_POWERED_OFF || state->screen == APP_SCREEN_BOOT) return;
    if(state->screen == APP_SCREEN_SLEEP) {
        if(action == UI_ACTION_OK) app_state_device_wake(state);
        return;
    }
    if(state->notice != APP_NOTICE_NONE) {
        if(action == UI_ACTION_OK || action == UI_ACTION_BACK) app_state_notice_dismiss(state);
        return;
    }
    if(state->power_confirmation) {
        if(action == UI_ACTION_BACK) app_state_power_cancel(state);
        return;
    }
    if(state->screen == APP_SCREEN_CHARGING) return;

    if(state->screen == APP_SCREEN_BRIGHTNESS) {
        if(action == UI_ACTION_UP && state->brightness < 4U) ++state->brightness;
        else if(action == UI_ACTION_DOWN && state->brightness > 1U) --state->brightness;
        else if(action == UI_ACTION_OK || action == UI_ACTION_BACK) enter_home(state);
        return;
    }
    if(state->screen == APP_SCREEN_VOLUME) {
        if(action == UI_ACTION_UP && state->volume < 3U) ++state->volume;
        else if(action == UI_ACTION_DOWN && state->volume > 0U) --state->volume;
        else if(action == UI_ACTION_OK || action == UI_ACTION_BACK) enter_home(state);
        return;
    }
    if(state->screen == APP_SCREEN_BLUETOOTH) {
        if((action == UI_ACTION_LEFT || action == UI_ACTION_RIGHT ||
            action == UI_ACTION_UP || action == UI_ACTION_DOWN) && state->bluetooth == APP_BT_ON) {
            state->bluetooth = APP_BT_OFF;
            state->bt_progress = 0;
            state->bt_elapsed_ms = 0;
        }
        else if(action == UI_ACTION_OK || action == UI_ACTION_BACK) enter_home(state);
        return;
    }
    if(state->screen == APP_SCREEN_MODE_SELECT) {
        if(!mode_is_valid(state->workout_mode_selection)) state->workout_mode_selection = state->workout_mode;
        if(action == UI_ACTION_LEFT) {
            state->workout_mode_selection = state->workout_mode_selection == APP_WORKOUT_MODE_JUMP
                ? APP_WORKOUT_MODE_PLANK
                : (app_workout_mode_t)(state->workout_mode_selection - 1);
        }
        else if(action == UI_ACTION_RIGHT) {
            state->workout_mode_selection = (app_workout_mode_t)((state->workout_mode_selection + 1) % APP_WORKOUT_MODE_COUNT);
        }
        else if(action == UI_ACTION_OK) {
            state->workout_mode = state->workout_mode_selection;
            enter_diagram(state);
        }
        else if(action == UI_ACTION_BACK) app_state_cancel_mode_select(state);
        return;
    }
    if(state->screen == APP_SCREEN_DIAGRAM) {
        if(action == UI_ACTION_OK) enter_tilt(state);
        else if(action == UI_ACTION_BACK) enter_home(state);
        return;
    }
    if(state->screen == APP_SCREEN_TILT) {
        if(action == UI_ACTION_OK) enter_radar(state);
        else if(action == UI_ACTION_BACK) enter_home(state);
        return;
    }
    if(state->screen == APP_SCREEN_RADAR) {
        if(action == UI_ACTION_OK) enter_countdown(state);
        else if(action == UI_ACTION_BACK) enter_home(state);
        return;
    }
    if(state->screen == APP_SCREEN_COUNTDOWN) {
        if(action == UI_ACTION_BACK) enter_home(state);
        return;
    }
    if(state->screen == APP_SCREEN_WORKOUT) {
        if(action == UI_ACTION_OK) state->workout.paused = !state->workout.paused;
        else if(action == UI_ACTION_STOP || action == UI_ACTION_DOWN)
            app_state_workout_stop(state);
        return;
    }
    if(state->screen == APP_SCREEN_RESULT) {
        if(action == UI_ACTION_SYNC || action == UI_ACTION_LEFT)
            app_state_result_sync(state);
        else if(action == UI_ACTION_OK || action == UI_ACTION_BACK) app_state_result_done(state);
        return;
    }

    if(state->home_card < APP_HOME_CARD_WORKOUT || state->home_card >= APP_HOME_CARD_COUNT)
        state->home_card = APP_HOME_CARD_WORKOUT;
    if(action == UI_ACTION_LEFT) {
        state->home_card = state->home_card == APP_HOME_CARD_WORKOUT
            ? APP_HOME_CARD_BLUETOOTH : (app_home_card_t)(state->home_card - 1);
    }
    else if(action == UI_ACTION_RIGHT) {
        state->home_card = (app_home_card_t)((state->home_card + 1) % APP_HOME_CARD_COUNT);
    }
    else if(action == UI_ACTION_OK) {
        static const app_screen_t card_screens[APP_HOME_CARD_COUNT] = {
            APP_SCREEN_MODE_SELECT, APP_SCREEN_BRIGHTNESS, APP_SCREEN_VOLUME, APP_SCREEN_BLUETOOTH
        };
        if(state->home_card == APP_HOME_CARD_WORKOUT &&
           (state->charging || state->battery_percent < 10U)) return;
        state->screen = card_screens[state->home_card];
        if(state->screen == APP_SCREEN_MODE_SELECT) state->workout_mode_selection = state->workout_mode;
    }
}
