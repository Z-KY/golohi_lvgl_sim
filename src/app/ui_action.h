#ifndef GOLOHI_UI_ACTION_H
#define GOLOHI_UI_ACTION_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    UI_ACTION_LEFT = 0,
    UI_ACTION_RIGHT,
    UI_ACTION_UP,
    UI_ACTION_DOWN,
    UI_ACTION_OK,
    UI_ACTION_BACK,
    UI_ACTION_STOP,
    UI_ACTION_SYNC
} ui_action_t;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* GOLOHI_UI_ACTION_H */
