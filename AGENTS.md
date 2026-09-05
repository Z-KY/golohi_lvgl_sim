# Golohi LVGL Simulator

## Goal

Implement the Golohi touchscreen UI prototype in LVGL.

The visual and interaction reference is:

    spec/UI.html

The simulator target is a 320x240 landscape display.

The final embedded target will be ESP32-S3 + 320x240 touch LCD,
so shared UI/application code must remain portable to ESP-IDF/LVGL.

## Project structure

Third-party / platform code:

    lvgl/
    FreeRTOS/
    src/hal/

Application code:

    src/app/

LVGL UI code:

    src/ui/

Images/fonts/icons:

    src/assets/

Do NOT modify files under:

    lvgl/
    FreeRTOS/

unless explicitly requested.

## Architecture

Keep UI rendering separate from application state.

Application actions should use an abstraction similar to:

    UI_ACTION_LEFT
    UI_ACTION_RIGHT
    UI_ACTION_UP
    UI_ACTION_DOWN
    UI_ACTION_OK

PC keyboard, mouse/touch gestures, and future ESP32 touch input
should ultimately dispatch the same application actions.

Do not put application state directly into SDL-specific code.

Shared code under src/app and src/ui must not depend directly on SDL.

## Simulator

Target resolution:

    320 x 240 landscape

Use LVGL software rendering.

Current configuration intentionally disables runtime vector graphics.
Do not introduce ThorVG/SVG runtime rendering unless explicitly requested.

Prefer normal LVGL objects:

    lv_obj
    lv_label
    lv_image
    flex/grid
    lv_timer
    LVGL events

## HTML reference

The 320x240 `.screen` element in spec/UI.html represents the real LCD.

The HTML D-pad and Demo Controls are development controls outside the
physical 320x240 screen and should NOT be copied into the product LCD UI.

The Home screen contains:

    status bar
    one large bigCard
    four page indicator dots

Home cards:

    Workout mode
    Brightness
    Volume
    Bluetooth

The first implementation target is only the Home screen and bigCard.

## Portability

The future target is ESP32-S3.

Avoid:
- desktop-only APIs in src/app and src/ui
- dynamic allocation outside normal LVGL usage when unnecessary
- large runtime SVG/vector dependencies
- unnecessary transparency/blur/shadow effects
- assumptions about mouse or keyboard in shared UI code

## Build

From repository root:

    cmake --build build -j$(nproc)

Executable:

    ./bin/main

If CMake configuration needs to be regenerated:

    rm -rf build
    mkdir build
    cd build
    cmake ..

## Working rules

Before changing code:

1. Inspect the existing implementation.
2. Read the relevant section of spec/UI.html.
3. State the files that will be changed.
4. Keep changes small and buildable.

After every implementation step:

1. Build the simulator.
2. Fix compiler errors.
3. Report which files changed.
4. Report what remains different from spec/UI.html.

Do not attempt the entire UI in one change.
Implement milestone-by-milestone.