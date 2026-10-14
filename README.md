# 基于 STM32 的四轴飞行器

> 基于 STM32F103 的四旋翼无人机飞控端与遥控端工程

本仓库包含飞控端与遥控端的完整 Keil MDK 工程源码、硬件设计资料与实验数据。

## 目录结构

| 路径 | 说明 |
| --- | --- |
| `fly_lib/` | **飞控端**（四轴飞行器主控）Keil MDK 工程，STM32F103 |
| `remote_lib/` | **遥控端**（手持遥控器）Keil MDK 工程，STM32F103 |
| `图纸/` | 原理图、PCB 等硬件图纸 |
| `实验数据/` | 飞行测试与调试实验数据 |
| `硬件设计.zip` | 硬件设计源文件打包 |

## 源码结构（以 `fly_lib/` 为例）

```
fly_lib/
├── User/          # 主程序入口、任务调度、中断服务
├── HAL/           # 应用层：控制算法 Control.c、遥控解析 Remote.c、匿名上位机 ANO_DT.c
├── Math/          # 姿态解算与滤波：imu.c、kalman.c、filter.c、pid.c
├── Hardware/      # 外设驱动：MPU6050、NRF24L01、OLED、LED
├── Config/        # 片上外设配置：GPIO、TIM、USART、SPI、IIC、ADC、USB
├── Library/       # STM32F10x 标准外设库
├── Start/         # 启动文件、CMSIS 内核文件
├── USB_hid/       # USB HID 设备类实现
├── USB_library/   # USB 底层驱动库
├── RTE/           # Keil 运行时环境配置
└── project.uvprojx
```

## 开发环境

- 芯片：STM32F103
- IDE：Keil MDK 5（uVision）
- 库：STM32F10x 标准外设库
- 调试器：J-Link

使用 Keil MDK 打开 `fly_lib/project.uvprojx` 或 `remote_lib/project.uvprojx` 即可编译。

## 说明

- 本仓库只收录**源码、硬件设计资料与实验数据**。原始目录中的 `White_flyer/` 参考资料手册包（约 5.9 GB，含教学视频、Keil 安装程序、匿名上位机等）体积远超 GitHub 限制，未纳入版本管理。
- 原始目录中 `代码/fly_lib`、`代码/remote_lib` 是与顶层重复的旧版本代码副本，本仓库按下列时间取用较新的一份：
  - 飞控端 `fly_lib` 取自顶层（2026-05，较新）
  - 遥控端 `remote_lib` 取自 `代码/`（2026-03，较新）
- Keil 编译产物（`Objects/`、`Listings/` 等）与 Office 临时文件已通过 `.gitignore` 排除。
