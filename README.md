# UniTrack - GPS+IMU 融合的跨场景全维度运动轨迹与状态监测系统

[![STM32](https://img.shields.io/badge/STM32-F407-blue.svg)](https://www.st.com/en/microcontrollers-microprocessors/stm32f407-417.html)
[![GPS](https://img.shields.io/badge/GPS-AT6558-green.svg)](https://github.com)
[![IMU](https://img.shields.io/badge/IMU-MPU6050-orange.svg)](https://github.com)
[![License](https://img.shields.io/badge/License-Academic-red.svg)](https://github.com)

## 📝 项目简介

**UniTrack** 是一个基于 STM32F407 微控制器的**通用智能运动记录系统**，通过 GPS 和 IMU 传感器融合，实现对多种运动场景的高精度轨迹记录与姿态监测。

### 核心特性
- 🛰️ **GPS 定位**: AT6558 模块，支持 GPS/BDS 双模
- 🧭 **IMU 姿态感知**: MPU6050 六轴传感器 + DMP
- 💾 **数据持久化**: SD 卡存储，支持断电保护
- 🖥️ **实时显示**: LVGL 图形界面，480×800 LCD
- 📊 **数据可视化**: 网页端轨迹分析与回放

### 应用场景
- 🚗 车载黑匣子
- 🚴 自行车码表
- 🏃 运动记录仪
- 📦 物流追踪
- 🛴 其他运动场景

## 🏗️ 项目结构

```
SmartMotionTracker/
├── APP/                    # 应用层代码
│   ├── app_ui_main.c      # LVGL 主界面
│   ├── app_ui_splash.c    # 启动界面
│   ├── app_gps.c          # GPS 模块驱动
│   ├── app_mpu.c          # MPU6050 驱动
│   └── app_sdcard.c       # SD 卡文件系统
├── HARDWARE/              # 硬件驱动层
│   ├── GPS/               # GPS 模块
│   ├── MPU/               # IMU 传感器
│   ├── LCD/               # 液晶显示
│   ├── SDIO/              # SD 卡接口
│   └── ...
├── SYSTEM/                # 系统层
│   ├── delay/             # 延时函数
│   ├── sys/               # 系统配置
│   └── usart/             # 串口通信
├── USER/                  # 用户层
│   ├── main.c             # 主程序
│   └── template.uvprojx   # Keil 工程文件
├── LVGL/                  # LVGL 图形库 v5.3
├── FATFS/                 # FAT 文件系统
├── visualization/         # 📊 数据可视化工具
│   ├── index.html         # 网页可视化界面
│   └── README.md          # 使用说明
└── README.md              # 本文件
```

## 🔧 硬件配置

### 主控板
- **MCU**: STM32F407ZGT6
- **主频**: 168 MHz
- **Flash**: 1 MB
- **SRAM**: 192 KB

### 外设模块
| 模块 | 型号 | 接口 | 功能 |
|------|------|------|------|
| GPS | AT6558 | UART | GPS/BDS 双模定位 |
| IMU | MPU6050 | I2C | 六轴姿态传感器 |
| LCD | ILI9488 | FSMC | 480×800 TFT 显示 |
| SD卡 | - | SDIO | 数据存储 |

### 引脚配置
```c
GPS:  USART3 (PB10/PB11)
MPU:  I2C1 (PB8/PB9)
LCD:  FSMC (16-bit parallel)
SDIO: PC8-PC12, PD2
```

## 🚀 快速开始

### 环境要求
- **IDE**: Keil MDK-ARM v5.x
- **编译器**: ARMCC v5 或 ARMCC v6
- **烧录器**: ST-Link V2
- **调试工具**: USART串口 (115200 baud)

### 编译步骤
1. 打开 `USER/template.uvprojx`
2. 选择目标平台: `STM32F407ZGTx`
3. 点击 Build 或按 F7 编译
4. 使用 ST-Link 下载到开发板

### 首次运行
1. 插入 FAT32 格式的 SD 卡
2. 连接 GPS 天线（室外环境）
3. 上电启动，等待 GPS 定位
4. 进入 `Files` 标签页
5. 点击 `Start Log` 开始记录

## 📊 功能说明

### 1. Home 页面
- 项目介绍
- 功能列表
- 快速使用指南

### 2. GPS 页面
实时显示 GPS 数据：
- 经纬度坐标
- 速度（km/h）
- 海拔高度
- 卫星数量
- UTC/北京时间

### 3. IMU 页面
实时显示姿态数据：
- Pitch（俯仰角）
- Roll（横滚角）
- Yaw（偏航角）
- 温度

### 4. Files 页面
数据管理与记录：
- **Start Log**: 开始记录（绿色按钮）
- **Stop Log**: 停止记录（红色按钮）
- **Refresh**: 刷新文件列表
- **Delete All**: 清空所有数据
- 实时统计：速度、距离、运动状态
- 自动扫描已有文件

### 5. 黑夜模式
每个页面右下角有 Dark/Light 切换按钮

## 💾 数据格式

### CSV 文件结构
```csv
Time,Lat,LatDir,Lon,LonDir,Speed,Alt,Sats,Pitch,Roll,Yaw,Temp
14:23:45,32.123456,N,118.234567,E,45.2,100.5,8,2.3,-1.5,90.2,25.3
```

### 文件命名规则
- `data_000.csv` - 第一次记录
- `data_001.csv` - 第二次记录
- `data_XXX.csv` - 自动递增编号

### 数据持久化
- ✅ 断电数据保护（SD 卡存储）
- ✅ 多文件管理（避免覆盖）
- ✅ 自动索引递增

## 🌐 数据可视化

### 网页端分析工具
进入 `visualization/` 目录，使用浏览器打开 `index.html`

**功能列表：**
- 🗺️ 地图轨迹显示（基于 Leaflet.js）
- 🎨 速度热图（颜色编码）
- 📊 统计分析（距离、速度、时长）
- 📈 速度曲线图（Chart.js）
- ▶️ 轨迹回放动画
- 💾 报告导出（TXT 格式）

**使用方法：**
1. 从 SD 卡读取 CSV 文件
2. 拖拽到网页上传区域
3. 自动生成可视化分析

详见: [visualization/README.md](visualization/README.md)

## 🔬 技术亮点

### 1. GPS+IMU 数据融合
- GPS 提供绝对位置
- IMU 补充姿态信息
- 互补滤波提高精度

### 2. 多传感器协同
```c
// 数据采集周期
GPS:  1Hz  (NMEA 解析)
IMU:  50Hz (DMP 输出)
SD卡: 1Hz  (CSV 写入)
UI:   10Hz (界面刷新)
```

### 3. 文件系统优化
- 使用 `FA_CREATE_NEW` 避免覆盖
- 启动时扫描已有文件
- 自动分配下一个可用索引

### 4. UI/UX 设计
- LVGL 5.3 图形库
- Tab 式多页面布局
- 深色/浅色主题切换
- 文件管理器风格界面

## 📈 性能指标

| 指标 | 数值 |
|------|------|
| GPS 精度 | 2.5m CEP (室外开阔) |
| IMU 精度 | ±1° (DMP 模式) |
| 采样频率 | 1 Hz |
| 存储容量 | 取决于 SD 卡大小 |
| 续航时间 | 约 8 小时 (2000mAh) |
| LCD 响应 | 60 FPS |

## 🛠️ 开发工具链

- **Keil MDK**: v5.38
- **ST-Link**: v2/v3
- **串口工具**: Xshell / PuTTY
- **地图服务**: OpenStreetMap
- **版本控制**: Git + GitHub

## 📚 相关文档

- [STM32F407 数据手册](https://www.st.com/resource/en/datasheet/stm32f407vg.pdf)
- [AT6558 用户手册](https://github.com)
- [MPU6050 寄存器手册](https://invensense.tdk.com/products/motion-tracking/6-axis/mpu-6050/)
- [LVGL 官方文档](https://docs.lvgl.io/5.3/)
- [FATFS 文档](http://elm-chan.org/fsw/ff/00index_e.html)

## 🐛 已知问题

- [ ] GPS 冷启动需要 30-60 秒
- [ ] 室内环境 GPS 无法定位
- [ ] SD 卡热插拔可能导致文件损坏

## 🔄 更新日志

### v1.0.0 (2025-01)
- ✅ 完成 GPS + IMU 数据采集
- ✅ 实现 SD 卡数据存储
- ✅ 开发 LVGL 用户界面
- ✅ 添加网页可视化工具
- ✅ 支持多文件管理
- ✅ 添加深色模式

## 👥 贡献者

- **开发者**: XCMB-haochi
- **学校**: Jiangsu University
- **课程**: 嵌入式系统设计
- **时间**: 2025 年 1 月

## 📧 联系方式

- **GitHub**: [@XCMB-haochi](https://github.com/XCMB-haochi)
- **仓库**: [2025-ARM-MidTerm-HW-SmartMotionTracker](https://github.com/XCMB-haochi/2025-ARM-MidTerm-HW-SmartMotionTracker)

## 📄 许可证

本项目仅用于**学术研究**和**课程设计**，未经许可不得商用。

---

**课程设计题目**: 《UniTrack：GPS+IMU 融合的跨场景全维度运动轨迹与状态监测系统设计》

**开发时间**: 2025 年 1 月
**最后更新**: 2025-01-XX
