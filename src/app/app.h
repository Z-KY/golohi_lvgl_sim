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

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* GOLOHI_APP_H */
