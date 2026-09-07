# Golohi UI 移植到 ESP32-S3 交接说明

## 1. 文档目的

本文说明当前 PC 模拟器距离运行在 ESP32-S3 + 320×240 触摸屏上还缺少哪些工作，以及建议的实施顺序和验收条件。

文档基于仓库提交 `a5cf629` 的代码现状。硬件型号、引脚和 ESP-IDF 版本尚未在仓库中定义，因此涉及具体驱动 API、时钟和缓存大小的数值必须在拿到原理图、屏幕规格书和传感器规格书后确定。

## 2. 当前结论

`src/app/` 与 `src/ui/` 已经具备较好的移植基础：

- 分辨率按 320×240 设计，颜色深度配置为 RGB565（`LV_COLOR_DEPTH 16`）。
- 应用状态集中在 `app_state_t`，输入统一为 `ui_action_t` 或语义化设备事件。
- UI 使用普通 LVGL 对象、事件和定时器，没有依赖 SDL、POSIX、文件系统或运行时 SVG/ThorVG。
- SDL 和 PC 键盘、传感器模拟集中在 `src/hal/`，共享层没有直接包含 SDL 头文件。

但当前仓库仍是 PC 模拟器，**不能直接用 ESP-IDF 编译或烧录**。主要缺口是 ESP-IDF 工程骨架、真实显示/触摸驱动、真实传感器及设备服务、线程模型、功耗和硬件验收。

特别注意：

- `src/main.c` 是 POSIX/SDL 主循环，只能用于 PC。
- `src/hal/hal.c` 是 SDL 显示、鼠标、键盘及模拟传感器实现，只能用于 PC。
- `src/freertos_main.c` 和 `src/freertos/freertos_posix_port.c` 是上游 PC FreeRTOS 示例，仍会创建 SDL 320×480 窗口和 Hello World 页面；它们不是 ESP32-S3 移植入口，不应复制到固件。
- 当前亮度、音量、蓝牙、充电、电池、倾角、雷达和运动计数大多只改变内存状态或由模拟器定时器生成，并未连接真实硬件。

## 3. 移植前必须确认的输入资料

以下资料缺少任何一项，都可能导致驱动层返工：

- ESP32-S3 模组型号、Flash/PSRAM 容量和是否启用 octal 模式。
- LCD 控制器型号、接口类型（RGB、SPI、8080 等）、分辨率、像素格式、刷新时序和背光电路。
- 触摸控制器型号、接口、IRQ/RESET 引脚、坐标范围、屏幕旋转方向和校准要求。
- LCD、触摸、I²C/SPI、背光 PWM、电源、充电检测、电池 ADC、按键/编码器的完整引脚表。
- LIS3DH 或最终姿态传感器、雷达/测距传感器的型号、采样率和业务算法接口。
- 音频器件及接口、提示音资源、音量控制方式。
- 蓝牙功能边界：仅配对状态、BLE 数据同步，还是需要手机协议、加密、重传和升级。
- 睡眠/唤醒源、关机电路、充电时可用功能和低电量阈值。
- 固件所用 ESP-IDF、LVGL 和组件版本。当前 LVGL 子模块提交为 `c016f72d...`，移植工程应锁定并验证兼容版本，不能同时混用两套 LVGL。

建议先把这些信息整理为一份 `board_config.h` 或 Kconfig 配置，再开始驱动编码。

## 4. 必须完成的工作

### 阶段 A：建立 ESP-IDF 可编译工程（P0）

- 新建 ESP-IDF 工程入口 `app_main()`，不要复用 PC 的 `main()` 或 `freertos_main.c`。
- 将 `src/app/*.c` 和 `src/ui/*.c` 组织为一个或两个 ESP-IDF component。
- 为板级代码单独建立组件或目录，例如 `components/board/`；共享代码继续保持不依赖 ESP-IDF 驱动头文件。
- 选择一种 LVGL 引入方式：ESP-IDF component、managed component 或仓库子模块，只保留一种，并锁定版本。
- 把所需 LVGL 配置移入目标工程，至少保持 320×240、RGB565、软件渲染、禁用运行时矢量图。
- 建立 Debug/Release 的 `sdkconfig.defaults`，记录 PSRAM、Flash、日志级别和优化选项。

验收：执行 `idf.py build` 能生成固件，链接结果中不出现 SDL、pthread、POSIX 主循环或 PC mock 源文件。

### 阶段 B：完成 LVGL 板级移植（P0）

- 初始化 LCD 总线和面板，注册 LVGL display flush 回调。
- 按接口能力选择单缓冲或双缓冲，确认缓冲区位于 DMA 可访问内存；PSRAM 是否可直接 DMA 由具体 LCD 外设和 IDF 配置决定。
- 在 flush 完成后准确调用 LVGL 的刷新完成接口，处理异步 DMA 回调与 cache 对齐要求。
- 确认 RGB565 字节序、颜色是否颠倒、横屏旋转以及逻辑坐标确实为 320×240。
- 提供 LVGL 毫秒时基，并创建唯一的 LVGL/UI 任务调用 `lv_timer_handler()`。
- 明确 LVGL 线程安全策略：所有 LVGL API 和 `ui_app_render()` 默认只在同一个 UI 任务中调用；其他任务通过队列投递事件。
- 初始化电容/电阻触摸，注册 pointer indev 回调，完成坐标缩放、旋转、去抖和边缘校准。

验收：开机页和 Home 页可持续刷新；触摸点击及左右滑动坐标正确；连续运行无撕裂、花屏、看门狗复位或 flush 卡死。

### 阶段 C：接入统一输入与跨任务事件（P0）

- 触摸继续走 LVGL 事件；实体按键、编码器或遥控输入转换为 `UI_ACTION_LEFT/RIGHT/UP/DOWN/OK/BACK/STOP/SYNC`。
- 建立固定长度 FreeRTOS 队列，将传感器任务、BLE 回调、电源回调产生的事件发送到 UI 任务。
- 禁止从中断、BLE 回调或传感器任务直接调用 LVGL，也不要在多个任务中直接修改 `app_state_t`。
- 规定队列满时的策略，并对不可丢事件（低电量、充电、错误、停止运动）做优先级或合并处理。
- 映射唤醒输入：当前睡眠页点击会调用 `app_device_wake()`，深睡后的真实唤醒应重新初始化硬件并恢复必要状态。

验收：触摸和实体输入触发相同状态迁移；高频传感器数据不会阻塞 UI 或导致竞态。

### 阶段 D：用真实设备服务替换 PC mock（P0/P1）

当前 `src/hal/hal.c:sensor_mock_timer_cb()` 会自动生成倾角、雷达进度、运动次数和动作提示。目标固件必须删除这条数据源，并逐项接入真实服务：

| 功能 | 当前接口/状态 | 目标实现 |
| --- | --- | --- |
| 倾角 | `app_submit_tilt_measurement()` | 传感器驱动、校准、滤波、角度换算和无效样本处理 |
| 雷达定位 | `app_submit_radar_progress()` | 雷达驱动及“定位进度/锁定”算法 |
| 运动计数 | `app_workout_add_rep()` | 实际动作识别算法；平板支撑模式不应累计次数 |
| 动作提示 | `app_workout_set_form_tip()` | 识别算法输出及提示节流 |
| 电池 | `app_device_set_battery()` | ADC/电量计采样、滤波、百分比曲线 |
| 充电 | `app_device_set_charging()` | 充电芯片或 GPIO 状态；当前代码中的快速自动涨电量仅为演示 |
| 亮度 | `state->brightness` | 映射到背光 PWM，占空比和最小可见亮度需实机标定 |
| 音量 | `state->volume` | 映射到音频驱动，并实现倒计时和反馈提示音 |
| 蓝牙 | `APP_BT_*` | BLE 连接/传输异步状态、失败/超时、协议及数据持久化 |
| 传感器错误 | `app_device_sensor_error()` | 启动自检、运行时故障和恢复策略 |

驱动层应提交“业务语义结果”，不要让 `src/ui/` 解析原始 I²C/SPI 数据。

验收：关闭所有 mock 后，完整流程仍能由真实数据完成：模式选择 → 摆放提示 → 倾角校准 → 雷达定位 → 倒计时 → 运动 → 结果 → 同步。

### 阶段 E：补齐设备行为（P1）

- 亮度、音量、最近模式等设置写入 NVS；定义恢复默认值和数据版本升级策略。
- 结果数据采用固定结构持久化，BLE 同步成功后再标记已上传，断连或复位不能静默丢失数据。
- 将“关机”状态连接到实际电源保持/深睡流程；保存必要数据后再关闭背光和外设。
- 实现自动休眠、唤醒源、充电插拔、低电量限制和 brownout 条件下的数据安全。
- 明确 boot、强制重启、RESET 和 OTA/工厂复位各自语义。当前 `app_device_reset()` 只重置 RAM 状态。
- 为 BLE、传感器、NVS、音频和电源操作增加错误返回、超时及用户提示。

验收：断电重启后设置和未同步结果符合产品定义；睡眠电流、唤醒路径和充电场景符合硬件指标。

### 阶段 F：资源、性能和稳定性验证（P1）

- 实机逐页对照 `spec/UI.html`，验证字号、间距、颜色、触摸热区和 320×240 边界；HTML 屏幕外的 D-pad/Demo Controls 不属于产品 UI。
- 统计静态 RAM、内部 DMA RAM、PSRAM、任务栈、Flash 和 LVGL 堆峰值。当前 `LV_MEM_SIZE` 为 160 KiB，只能作为起点，需以实测为准。
- 检查全部启用字体。当前仅启用 Montserrat 12/14/20/32；若加入中文或自定义字形，应采用裁剪后的静态字体资源并重新评估 Flash/RAM。
- 在目标帧率下测量 flush 时间、CPU 占用和最差输入延迟，必要时调整 draw buffer、刷新周期和脏区策略。
- 做长稳、快速连点/滑动、反复进出页面、BLE 断连、传感器超时、低电量、充电插拔、休眠唤醒和看门狗测试。
- 增加 `app_state` 的主机单元测试，覆盖状态迁移、边界值和大时间步进；硬件层增加板级冒烟测试。

验收：无明显内存增长或栈溢出；异常路径可恢复；实机视觉和交互差异形成已关闭或已批准的清单。

## 5. 推荐目录方案

ESP-IDF 工程可以独立建立，复用本仓库共享源码；下面只表示职责，不强制目录名称：

```text
firmware/
├── CMakeLists.txt
├── sdkconfig.defaults
├── main/
│   ├── CMakeLists.txt
│   └── main.c                 # app_main、系统初始化和任务创建
└── components/
    ├── golohi_app/            # 复用 src/app
    ├── golohi_ui/             # 复用 src/ui
    ├── board/                 # LCD、触摸、背光、按键、电源
    ├── sensor_service/        # 倾角、雷达、动作识别
    ├── bluetooth_service/     # BLE 协议与同步
    └── audio_service/         # 提示音与音量
```

共享代码可以通过复制、Git 子模块或 CMake 指向公共路径引入。无论采用哪种方式，都要保证 PC 模拟器和固件实际编译的是同一份 `src/app`、`src/ui`，避免形成两套分叉源码。

## 6. 建议的初始化和运行顺序

1. 初始化 NVS、事件队列和设备服务。
2. 初始化 LVGL 时基、LCD、显示缓冲和触摸输入。
3. 在 UI 任务中调用 `app_init()`，再调用 `ui_app_init()`。
4. 启动传感器、BLE、电池和电源任务；它们只向 UI 任务发送语义事件或最新采样。
5. UI 任务消费事件，调用 `app_*`，状态改变后调用 `ui_app_render()`。
6. UI 任务持续调用 `lv_timer_handler()`，并按返回值做有上下限的延时或通知等待。

## 7. 上板前后的验收清单

### 首次点亮

- [ ] `idf.py build/flash/monitor` 可重复执行。
- [ ] LCD 为 320×240 横屏，颜色、字节序和刷新方向正确。
- [ ] LVGL tick、flush 和触摸 indev 工作正常。
- [ ] 启动页 2 秒后进入 Home，四张卡片可切换。

### 完整功能

- [ ] 亮度真实控制背光，并在重启后恢复。
- [ ] 音量真实影响提示音，并在重启后恢复。
- [ ] BLE 可配对、传输、失败重试，结果不会丢失。
- [ ] 倾角和雷达校准由真实传感器驱动，60 秒超时可恢复。
- [ ] 六种运动模式均完成一次端到端流程。
- [ ] 电池、充电、低电量、休眠、唤醒和关机符合产品定义。
- [ ] 所有异常弹窗和物理输入均可恢复到明确状态。

### 发布门槛

- [ ] Release 配置下无编译告警和未处理的关键错误返回。
- [ ] 任务栈、LVGL 堆、DMA 内存和 PSRAM 有足够余量。
- [ ] 长时间运行、频繁交互和充电/断连压力测试通过。
- [ ] 固件版本、LVGL/ESP-IDF 版本、引脚表和烧录方法已写入发布说明。
- [ ] PC 模拟器与固件的共享状态机测试结果一致。

## 8. 不应带入目标固件的文件

以下文件属于 PC 平台或上游模拟示例，不应加入 ESP-IDF component 源列表：

```text
src/main.c
src/hal/hal.c
src/hal/hal.h
src/mouse_cursor_icon.c
src/freertos_main.c
src/freertos/freertos_posix_port.c
CMakeLists.txt                 # 这是 PC/SDL 构建入口，可参考但不能直接复用
```

`lvgl/` 和 `FreeRTOS/` 也不建议直接复制进 ESP-IDF 工程：ESP-IDF 已提供 FreeRTOS，LVGL 应通过选定且锁定的单一组件来源引入。
