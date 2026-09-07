# Golohi LVGL 模拟器代码结构说明

## 1. 项目定位

本仓库是在 Linux/WSL 上运行的 320×240 LVGL PC 模拟器，用 `spec/UI.html` 作为视觉和交互参考。当前代码不仅包含 Home 大卡片，也包含运动选择、校准、倒计时、运动、结果、设备生命周期和异常弹窗等页面。

代码按“应用状态、LVGL 界面、PC 平台适配”拆分。移植到 ESP32-S3 时，主要复用 `src/app/` 和 `src/ui/`，替换入口与 HAL。

## 2. 目录总览

```text
.
├── CMakeLists.txt             # PC/SDL 构建配置与源文件清单
├── lv_conf.h                  # 当前 LVGL 配置：RGB565、软件渲染等
├── spec/UI.html               # 320×240 产品 UI 的视觉/交互参考
├── src/
│   ├── main.c                 # 非 FreeRTOS 的 PC 入口与 LVGL 主循环
│   ├── freertos_main.c        # 上游 PC FreeRTOS 示例，不是当前产品入口
│   ├── freertos/              # POSIX FreeRTOS 适配辅助代码
│   ├── mouse_cursor_icon.c    # PC 鼠标光标资源
│   ├── hal/                   # SDL 显示、鼠标/键盘和传感器 mock
│   ├── app/                   # 与 LVGL/SDL 无关的应用状态和状态机
│   ├── ui/                    # LVGL 页面、主题、事件和渲染
│   └── assets/                # 预留的图片、字体、图标目录，目前为空
├── lvgl/                      # LVGL 第三方子模块，不修改
└── FreeRTOS/                  # PC 模拟用 FreeRTOS 子模块，不修改
```

根目录 `README.md` 主要保留自上游 LVGL PC 模拟器模板，产品代码行为应以 `src/`、`CMakeLists.txt`、`spec/UI.html` 和本文为准。

## 3. 分层与依赖方向

```text
PC main / PC HAL
       │
       ├──────────────┐
       ▼              ▼
  app 公共接口 ◄── LVGL UI
       │              │
       ▼              ▼
  app_state 状态机   LVGL 对象/事件
```

- `app_state` 是业务核心，不依赖 LVGL、SDL 或 ESP-IDF。
- `app` 持有唯一的全局 `app_state_t`，向 UI 和平台代码提供较稳定的接口。
- `ui` 读取状态、创建 LVGL 对象，并在触摸事件中调用 `app_*`。
- `hal` 将 PC 键盘和模拟设备事件转换为 `app_*` 调用，同时创建 SDL 显示和输入设备。
- `app` 不反向调用 `ui`；状态改变后，由事件发起方或 `ui_app` 定时器触发 `ui_app_render()`。

这条依赖方向是移植边界。ESP32-S3 的显示、触摸和传感器代码不应放进 `src/app/` 或 `src/ui/`。

## 4. 启动和运行流程

当前默认构建未启用 PC FreeRTOS，入口是 `src/main.c`：

1. `lv_init()` 初始化 LVGL。
2. `sdl_hal_init(320, 240)` 创建 SDL 窗口、鼠标、滚轮、键盘事件队列和传感器 mock 定时器。
3. `app_init()` 初始化唯一的应用状态，初始页面为 `APP_SCREEN_BOOT`。
4. `ui_app_init()` 创建所有页面对象，建立 40 ms 应用 tick 定时器，并首次渲染。
5. 主循环调用 `lv_timer_handler()`，按其返回值休眠。
6. Boot 状态计时 2 秒后切到 Home，`ui_app_render()` 根据状态显示对应页面。

`ui_app_init()` 会一次性创建全部页面，页面切换时只设置/清除 `LV_OBJ_FLAG_HIDDEN`，不会反复销毁重建页面。这使页面切换简单、确定，但也意味着启动时会一次性占用全部对象内存。

## 5. 应用层：`src/app/`

### `ui_action.h`

定义平台无关的输入动作：

```text
LEFT  RIGHT  UP  DOWN  OK  BACK  STOP  SYNC
```

键盘、实体按键或未来触摸手势都应尽量转换为这些动作。对于倾角数值、雷达进度、电池百分比等带参数事件，则使用对应的 `app_*` 语义接口。

### `app_state.h` / `app_state.c`

这里定义全部业务数据和状态迁移，是最适合做主机单元测试的部分。

主要类型包括：

- `app_screen_t`：当前页面/设备状态。
- `app_home_card_t`：Home 的 Workout、Brightness、Volume、Bluetooth 四张卡片。
- `app_workout_mode_t`：Jump、Jumping Jacks、Boxing、Squats、Push-ups、Plank 六种模式。
- `app_bt_state_t`：关闭、配对中、已连接、传输中。
- `app_calibration_state_t`：校准中、通过、失败。
- `app_workout_session_t` 与 `app_workout_result_t`：本次运动与结果。
- `app_state_t`：将页面、设备状态、计时、进度和业务数据集中保存。

核心入口：

- `app_state_dispatch()`：按当前页面解释统一输入动作。
- `app_state_tick()`：推进 boot、蓝牙、摆放提示、校准、倒计时、运动和结果页计时。
- `app_state_submit_*()`：接收倾角或雷达等外部数据。
- `app_state_device_*()`：接收充电、睡眠、故障、重启等设备语义事件。

主要运动流程为：

```text
BOOT → HOME → MODE_SELECT → DIAGRAM → TILT → RADAR
                                      │        │
                                      └─失败重试┴─失败重试
             RESULT ← WORKOUT ← COUNTDOWN
                │
                ├─完成→ HOME
                └─同步→ BLUETOOTH
```

此外 `SLEEP`、`CHARGING`、`POWERED_OFF` 是设备生命周期页面，`notice` 和 `power_confirmation` 通过全局弹窗叠加显示。

### `app.h` / `app.c`

`app.c` 内部持有一个静态 `app_state_t`，把 `app_state_*` 封装成不需要传入 state 指针的公共函数。典型调用方式：

```c
app_dispatch(UI_ACTION_RIGHT);
ui_app_render();
```

`app_get_state()` 返回只读指针。当前设计默认所有状态修改都在同一 UI/LVGL 线程发生；如果目标固件有多个 FreeRTOS 任务，必须用队列把事件汇聚到 UI 任务，不能并发修改该对象。

## 6. UI 层：`src/ui/`

### 总控与主题

| 文件 | 职责 |
| --- | --- |
| `ui_app.c/.h` | 创建所有页面、40 ms 驱动业务 tick、控制页面显隐、调用各页 render |
| `ui_theme.c/.h` | 公共颜色和基础样式；集中维护屏幕、页面、卡片、标签等视觉规则 |
| `ui_back_button.c/.h` | 可复用返回按钮，点击后派发 `UI_ACTION_BACK` |

### 页面模块

| 文件 | 对应状态/功能 |
| --- | --- |
| `ui_home.c/.h` | Home 状态栏、大卡片、四个页码点、点击和左右滑动 |
| `ui_brightness.c/.h` | 1～4 级亮度，按钮和拖动调节 |
| `ui_volume.c/.h` | 0～3 级音量，按钮和拖动调节 |
| `ui_bluetooth.c/.h` | 蓝牙关闭、配对、已连接和传输进度 |
| `ui_mode_select.c/.h` | 六种运动模式选择弹层 |
| `ui_diagram.c/.h` | 设备摆放示意、尺寸提示、10 秒进度及跳过 |
| `ui_tilt.c/.h` | 倾角校准进度、通过和失败 |
| `ui_radar.c/.h` | 雷达定位进度、通过和失败 |
| `ui_countdown.c/.h` | 3、2、1 倒计时 |
| `ui_workout.c/.h` | 次数或平板支撑计时、暂停、停止和动作提示 |
| `ui_result.c/.h` | 运动结果、完成及蓝牙同步入口 |
| `ui_device_pages.c/.h` | Boot、Sleep、Charging、Powered Off 页面 |
| `ui_dialog.c/.h` | 上传成功、充满、低电量、传感器故障和关机确认弹窗 |

每个页面通常采用同一种模式：

- 头文件中的 `ui_xxx_t` 保存需要更新或显隐的 LVGL 对象指针。
- `ui_xxx_create()` 只负责创建对象、设置固定样式并绑定事件。
- `ui_xxx_render()` 根据 `const app_state_t *` 更新文字、进度和可见性。
- 事件回调调用 `app_*` 改状态，然后立即调用 `ui_app_render()`。

页面图形主要由 `lv_obj`、`lv_label`、`lv_line`、`lv_arc` 和 `lv_bar` 组合绘制，没有加载图片文件或运行时 SVG。

## 7. PC 平台层：`src/hal/`

`sdl_hal_init()` 完成：

- 创建 320×240 SDL 窗口和默认 LVGL group。
- 创建鼠标和滚轮 indev，并设置 PC 鼠标光标。
- 注册 SDL 键盘监听；方向键、Enter 和 Escape 转为 `ui_action_t`。
- 用长度 16 的环形队列跨越 SDL 事件线程，在 LVGL 定时器中安全消费事件。
- F1～F12 生成启动、充电、睡眠、故障、重启、关机等开发测试事件。
- 每 120 ms 运行传感器 mock，模拟倾角收敛、雷达进度、运动计数和动作提示。

这部分是理解目标硬件适配的参考，但不属于共享固件代码。ESP32-S3 上应使用 FreeRTOS 队列和真实驱动提供同样的业务语义。

## 8. 状态变化与渲染的数据流

### 用户操作

```text
SDL 键盘 / LVGL 触摸事件
        → app_dispatch() 或 app_*()
        → app_state_t 改变
        → ui_app_render()
        → 对应页面 render()
```

### 时间推进

```text
ui_app 的 40 ms LVGL timer
        → app_tick(elapsed_ms)
        → 若状态或显示数据改变，返回 true
        → ui_app_render()
```

### 模拟传感器

```text
hal 的 120 ms LVGL timer
        → app_submit_*()/app_workout_*()
        → ui_app_render()
```

`app_tick()` 使用实际经过的毫秒数，不假设回调一定准时。移植后仍应传入真实 elapsed time，避免任务调度抖动使倒计时失准。

## 9. 构建配置

默认 PC 构建命令：

```bash
cmake --build build -j$(nproc)
./bin/main
```

若需要重新配置：

```bash
rm -rf build
mkdir build
cd build
cmake ..
```

关键配置现状：

- LVGL 子模块由根 `CMakeLists.txt` 加入。
- SDL2 是 PC 必需依赖。
- 默认 `USE_FREERTOS=OFF`，因此编译 `src/main.c`。
- `LV_COLOR_DEPTH=16`，`LV_MEM_SIZE=160 KiB`，`LV_USE_OS=LV_OS_NONE`。
- 启用 Montserrat 12、14、20、32 字体。
- 禁用 LVGL examples、demos、ThorVG 和运行时 vector graphics。
- 应用/UI 源文件在 `MAIN_SOURCES` 中显式列出；新增 `.c` 文件后必须同步修改构建清单。

## 10. 如何修改或新增功能

### 修改现有业务规则

1. 先在 `app_state.h` 中确认状态或枚举是否已存在。
2. 在 `app_state.c` 修改状态迁移或 tick 规则，不在 UI 回调中复制业务判断。
3. 如需公共入口，在 `app.h/.c` 增加薄封装。
4. 在对应 `ui_xxx_render()` 呈现新状态。
5. 构建并走一遍受影响流程。

### 新增页面

1. 给 `app_screen_t` 增加页面状态，并在 `app_state.c` 定义进入/离开规则。
2. 新建 `ui_xxx.h/.c`，提供 `create()` 和 `render()`。
3. 在 `ui_app.c` 增加静态页面对象、创建、显隐和渲染分支。
4. 在根 `CMakeLists.txt` 的 `MAIN_SOURCES` 中加入新 `.c`。
5. 确认返回动作、弹窗覆盖、睡眠/充电等全局状态不会留下隐藏页面。

### 接入新的输入或传感器

1. 原始驱动数据在平台/服务层处理。
2. 无参数导航优先转为 `ui_action_t`。
3. 带数值数据新增明确的 `app_*`/`app_state_*` 接口。
4. 在 UI 线程修改状态并渲染，避免跨线程直接调用 LVGL。

## 11. 维护时的注意事项

- `app_state_t` 是当前唯一事实来源，不要在单个页面再保存一份业务状态。
- 页面结构体中的指针只指向 LVGL 对象，不应承载传感器或协议状态。
- `create()` 后对象长期存在；若改为动态销毁页面，要同时处理悬空指针和定时器回调。
- `ui_app_render()` 会渲染当前页和全局弹窗。新增全局覆盖层时要检查层级、点击穿透和 hidden 标志。
- 所有 LVGL 调用都应在同一线程或正确加锁。当前 PC 代码通过事件队列规避 SDL 回调线程直接操作 UI。
- `src/hal/hal.c` 的充电速度、传感器数据和运动次数都是演示数据，不能据此推导真实算法。
- `spec/UI.html` 中 `.screen` 才是物理 LCD；屏幕外的 D-pad 和 Demo Controls 只用于开发验证。
- 不修改 `lvgl/` 和 `FreeRTOS/`；上游升级应单独进行并完整回归。

ESP32-S3 的剩余工作、优先级和实机验收标准见 [ESP32S3_PORTING_HANDOVER.md](ESP32S3_PORTING_HANDOVER.md)。
