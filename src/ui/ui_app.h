#ifndef GOLOHI_UI_APP_H
#define GOLOHI_UI_APP_H

#define UI_APP_SCREEN_WIDTH  320
#define UI_APP_SCREEN_HEIGHT 240

#ifdef __cplusplus
extern "C" {
#endif

void ui_app_init(void);
void ui_app_render(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* GOLOHI_UI_APP_H */
