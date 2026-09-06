#include "hal.h"

#include <stdbool.h>
#include <SDL2/SDL.h>

#include "app/app.h"
#include "app/ui_action.h"
#include "ui/ui_app.h"

enum {
  KEYBOARD_ACTION_QUEUE_SIZE = 16
};

typedef enum {
  PC_EVENT_ACTION = 0,
  PC_EVENT_BOOT,
  PC_EVENT_HOME,
  PC_EVENT_START_WORKOUT,
  PC_EVENT_CHARGE_ON,
  PC_EVENT_CHARGE_OFF,
  PC_EVENT_SLEEP,
  PC_EVENT_LOW_BATTERY,
  PC_EVENT_SENSOR_ERROR,
  PC_EVENT_FORCE_REBOOT,
  PC_EVENT_TILT_FAIL,
  PC_EVENT_RADAR_FAIL,
  PC_EVENT_POWER_DIALOG,
  PC_EVENT_RESET
} pc_event_kind_t;

typedef struct {
  pc_event_kind_t kind;
  ui_action_t action;
} pc_event_t;

static pc_event_t keyboard_action_queue[KEYBOARD_ACTION_QUEUE_SIZE];
static unsigned int keyboard_action_head;
static unsigned int keyboard_action_tail;
static SDL_SpinLock keyboard_action_lock;
static bool keyboard_action_handler_initialized;
static app_screen_t mock_screen = APP_SCREEN_HOME;
static int16_t mock_tilt_degrees;
static uint32_t mock_workout_elapsed_ms;
static uint32_t mock_plank_tip_seconds;

static bool sdl_key_to_action(SDL_Keycode key, ui_action_t * action)
{
  switch(key) {
    case SDLK_LEFT:
      *action = UI_ACTION_LEFT;
      return true;
    case SDLK_RIGHT:
      *action = UI_ACTION_RIGHT;
      return true;
    case SDLK_UP:
      *action = UI_ACTION_UP;
      return true;
    case SDLK_DOWN:
      *action = UI_ACTION_DOWN;
      return true;
    case SDLK_RETURN:
    case SDLK_KP_ENTER:
      *action = UI_ACTION_OK;
      return true;
    case SDLK_ESCAPE:
      *action = UI_ACTION_BACK;
      return true;
    default:
      return false;
  }
}

static bool sdl_key_to_device_event(SDL_Keycode key, SDL_Keymod mod,
                                    pc_event_kind_t * kind)
{
  switch(key) {
    case SDLK_F1: *kind = PC_EVENT_BOOT; return true;
    case SDLK_F2: *kind = PC_EVENT_HOME; return true;
    case SDLK_F3: *kind = PC_EVENT_START_WORKOUT; return true;
    case SDLK_F4:
      *kind = (mod & KMOD_SHIFT) ? PC_EVENT_CHARGE_OFF : PC_EVENT_CHARGE_ON;
      return true;
    case SDLK_F5: *kind = PC_EVENT_SLEEP; return true;
    case SDLK_F6: *kind = PC_EVENT_LOW_BATTERY; return true;
    case SDLK_F7: *kind = PC_EVENT_SENSOR_ERROR; return true;
    case SDLK_F8: *kind = PC_EVENT_FORCE_REBOOT; return true;
    case SDLK_F9: *kind = PC_EVENT_TILT_FAIL; return true;
    case SDLK_F10: *kind = PC_EVENT_RADAR_FAIL; return true;
    case SDLK_F11: *kind = PC_EVENT_POWER_DIALOG; return true;
    case SDLK_F12: *kind = PC_EVENT_RESET; return true;
    default: return false;
  }
}

static int SDLCALL sdl_keyboard_event_watch(void * user_data, SDL_Event * event)
{
  pc_event_t queued = {0};
  unsigned int next_tail;

  (void)user_data;

  if(event->type != SDL_KEYDOWN || event->key.repeat) {
    return 1;
  }
  if(sdl_key_to_action(event->key.keysym.sym, &queued.action)) {
    queued.kind = PC_EVENT_ACTION;
  }
  else if(!sdl_key_to_device_event(event->key.keysym.sym,
                                   (SDL_Keymod)event->key.keysym.mod,
                                   &queued.kind)) return 1;

  /* SDL event watches can run outside the LVGL thread. Queue the action here
   * and let keyboard_action_timer_cb update application/UI state. */
  SDL_AtomicLock(&keyboard_action_lock);
  next_tail = (keyboard_action_tail + 1U) % KEYBOARD_ACTION_QUEUE_SIZE;
  if(next_tail != keyboard_action_head) {
    keyboard_action_queue[keyboard_action_tail] = queued;
    keyboard_action_tail = next_tail;
  }
  SDL_AtomicUnlock(&keyboard_action_lock);

  return 1;
}

static bool keyboard_action_pop(pc_event_t * event)
{
  bool has_action = false;

  SDL_AtomicLock(&keyboard_action_lock);
  if(keyboard_action_head != keyboard_action_tail) {
    *event = keyboard_action_queue[keyboard_action_head];
    keyboard_action_head =
        (keyboard_action_head + 1U) % KEYBOARD_ACTION_QUEUE_SIZE;
    has_action = true;
  }
  SDL_AtomicUnlock(&keyboard_action_lock);

  return has_action;
}

static void keyboard_action_timer_cb(lv_timer_t * timer)
{
  pc_event_t event;

  (void)timer;

  while(keyboard_action_pop(&event)) {
    switch(event.kind) {
      case PC_EVENT_ACTION: app_dispatch(event.action); break;
      case PC_EVENT_BOOT: app_device_boot(); break;
      case PC_EVENT_HOME: app_device_home(); break;
      case PC_EVENT_START_WORKOUT: app_device_start_workout(); break;
      case PC_EVENT_CHARGE_ON: app_device_set_charging(true); break;
      case PC_EVENT_CHARGE_OFF: app_device_set_charging(false); break;
      case PC_EVENT_SLEEP: app_device_sleep(); break;
      case PC_EVENT_LOW_BATTERY: app_device_trigger_low_battery(); break;
      case PC_EVENT_SENSOR_ERROR: app_device_sensor_error(); break;
      case PC_EVENT_FORCE_REBOOT: app_device_force_reboot(); break;
      case PC_EVENT_TILT_FAIL: app_device_tilt_fail(); break;
      case PC_EVENT_RADAR_FAIL: app_device_radar_fail(); break;
      case PC_EVENT_POWER_DIALOG: app_power_request(); break;
      case PC_EVENT_RESET: app_device_reset(); break;
    }
    ui_app_render();
  }
}

/* Deterministic PC-only semantic sensor source. Shared app/ui code is unaware
 * whether these samples come from this simulator or embedded sensors. */
static void sensor_mock_timer_cb(lv_timer_t * timer)
{
  const app_state_t * state = app_get_state();
  (void)timer;

  if(state->screen != mock_screen) {
    mock_screen = state->screen;
    mock_workout_elapsed_ms = 0;
    mock_plank_tip_seconds = 0;
    if(mock_screen == APP_SCREEN_TILT) mock_tilt_degrees = 0;
  }

  if(state->screen == APP_SCREEN_TILT &&
     state->tilt_state == APP_CALIBRATION_RUNNING) {
    int16_t delta = state->tilt_target_deg - mock_tilt_degrees;
    if(delta > 0) mock_tilt_degrees += delta > 3 ? delta / 3 : 1;
    else if(delta < 0) mock_tilt_degrees -= delta < -3 ? (-delta) / 3 : 1;
    if((delta >= -1) && (delta <= 1)) mock_tilt_degrees = state->tilt_target_deg;
    app_submit_tilt_measurement(mock_tilt_degrees);
    ui_app_render();
  }
  else if(state->screen == APP_SCREEN_RADAR &&
          state->radar_state == APP_CALIBRATION_RUNNING) {
    uint8_t next = state->radar_progress >= 86U
                       ? 100U : (uint8_t)(state->radar_progress + 14U);
    app_submit_radar_progress(next);
    ui_app_render();
  }
  else if(state->screen == APP_SCREEN_WORKOUT && !state->workout.paused) {
    mock_workout_elapsed_ms += 120U;
    if(mock_workout_elapsed_ms >= 960U) {
      mock_workout_elapsed_ms -= 960U;
      if(state->workout.mode == APP_WORKOUT_MODE_PLANK) {
        ++mock_plank_tip_seconds;
        if((mock_plank_tip_seconds % 4U) == 0U)
          app_workout_set_form_tip(APP_FORM_TIP_STRAIGHT_LINE);
      }
      else {
        app_workout_add_rep();
        state = app_get_state();
        if((state->workout.reps % 4U) == 0U)
          app_workout_set_form_tip(APP_FORM_TIP_FULL_RANGE);
      }
      ui_app_render();
    }
  }
}


lv_display_t * sdl_hal_init(int32_t w, int32_t h)
{

  lv_group_set_default(lv_group_create());

  lv_display_t * disp = lv_sdl_window_create(w, h);

  lv_indev_t * mouse = lv_sdl_mouse_create();
  lv_indev_set_group(mouse, lv_group_get_default());
  lv_indev_set_display(mouse, disp);
  lv_display_set_default(disp);
  /*Declare the image file.*/
  LV_IMAGE_DECLARE(mouse_cursor_icon); 
  lv_obj_t * cursor_obj;
  /*Create an image object for the cursor */
  cursor_obj = lv_image_create(lv_screen_active()); 
  /*Set the image source*/
  lv_image_set_src(cursor_obj, &mouse_cursor_icon);           
  /*Connect the image  object to the driver*/
  lv_indev_set_cursor(mouse, cursor_obj);             

  lv_indev_t * mousewheel = lv_sdl_mousewheel_create();
  lv_indev_set_display(mousewheel, disp);
  lv_indev_set_group(mousewheel, lv_group_get_default());

  if(!keyboard_action_handler_initialized) {
    SDL_AddEventWatch(sdl_keyboard_event_watch, NULL);
    lv_timer_create(keyboard_action_timer_cb, 5, NULL);
    lv_timer_create(sensor_mock_timer_cb, 120, NULL);
    keyboard_action_handler_initialized = true;
  }

  return disp;
}
