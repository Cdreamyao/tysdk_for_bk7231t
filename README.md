# tysdk_for_bk7231t
## 简介
tysdk_for_bk7231t 是WiFi&&BLE双模芯片BK7231T，基于涂鸦独创 IoT OS 操作系统级别的端云 SDK，提供了按照业务需求选择组件，定制 SDK 的能力，是涂鸦开发者的利剑，具备轻量级、互联互通、安全传输、组件丰富、快速开发等关键能力，有效降低开发门槛、缩短开发周期，可广泛应用于可穿戴设备、视频监控、中控网关、智能音箱、智能家电、智能门锁等智能家居领域设备。

tysdk_for_bk7231t是基于 Tuya Device OS 技术体系按照物联网 Wi-Fi 应用场景，进行裁剪、定制得到的。

## SDK 目录结构说明
涂鸦通用 Wi-Fi SDK的产物包目录说明如下表所示：

| 目录         | 子目录    | 描述                                            | 备注                                                         |
| ------------ | --------- | ----------------------------------------------- | ------------------------------------------------------------ |
| beken378     | /         | BK7231T驱动                                     |                                                            |
| project      | tuya_user | bk7231t_sample                                 | 一个示例demo                                              |
|              | tuya_iot  | 存放涂鸦通用 Wi-Fi SDK 的头文件和库文件         | 参照后续的功能接口说明                                       |
|              | tuya_common | 涂鸦通用 Wi-Fi SDK入口函数、涂鸦驱动文件       |                                                              |
| tools        | /         | USB下载工具、串口下载工具                       |                                                              |
| toolchain    | /         | 交叉编译环境                                   |                      |
| Makefile     | /         | 主makefile                                    |                                                              |
| application.mk |/        | 编译makefile                                  |                                                              |
| build_app.sh | /         | 客户应用、demo 的编译入口                       |                                                              |
| README.md    | /         | 介绍文档                                      |                                                              |


## 功能说明 

涂鸦通用 Wi-Fi SDK 包括了 Tuya Device OS 支持的，在 Wi-Fi 应用场景下常见的各种功能，包括配网、连接涂鸦云、激活、局域网控制、云端控制、红外、定时任务、在线日志、安全框架、系统服务等各种功能，如何使用这些功能，请参照《[涂鸦通用 Wi-Fi SDK 说明](https://docs.tuya.com/zh/iot/device-development/access-mode-link/wifi-chip-sdk/tuya-common-wifi-sdk?id=K9glcmvw4u9ml)》。



## 使用说明

涂鸦通用 Wi-Fi SDK 包括了库文件、编译工具链以及编译脚本，不需要用户进行任何其他的配置，直接就可以在当前的 SDK 产物目录环境下，进行开发、编译操作。同时也提供了Demo示例，演示如何快速使用该 SDK 进行嵌入式应用开发。具体细节，请参照《[BK7231T SDK使用及编译说明](./tools/README_Build.md)》。



## 烧录、授权与产测

涂鸦通用 Wi-Fi SDK 提供了产测、烧录和授权的能力，用户可以在涂鸦开发者平台申请授权信息，根据自己的开发模式，选择相应的合适的烧录、授权和产测方式。具体细节，请参照《[BK7231T 烧录、授权说明](./tools/README_Program.md)》。


## 当前SDK版本
Version:30.06