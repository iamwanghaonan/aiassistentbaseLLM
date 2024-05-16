#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// 替换为您的网络凭证
const char *ssid = "i-WZU";    //你的网络名称
//const char *password = "wifi密码"; //你的网络密码
 
const char *headerKeys[] = {"********", "********"};// 校园网的账号和密码
 
const char *http_get_com = "http://192.168.16.66:801/srun_portal_pc.php?ac_id=2&?ac_id=2&wlanuserip=10.218.151.29&wlanacname=WZDX-N18K&ssid=&nasip=10.216.128.1&snmpagentip=&mac=b03cdc974c1c&t=wireless-v2-plain&url=http://www.msftconnecttest.com/redirect&apmac=&nasid=WZDX-N18K&vid=2755&port=258&nasportid=TenGigabitEthernet%201/5/10.27550000:2755-0";// 对应的校园网网址

HTTPClient http; // 声明HTTPClient对象
// 替换为您的OpenAI API密钥
const char* apiKey = "empty";

// 发送请求到OpenAI API
String inputText = "你好，智谱！";
String apiUrl = "http://10.14.10.101:8000/v1";
String answer;

String getGPTAnswer(String inputText) {
    HTTPClient http;
    http.begin(apiUrl);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", String(apiKey));

    String payload = "{\"prompt\":{\"role\": \"user\",\"content\": \"" + inputText + "\"}}";
    int httpResponseCode = http.POST(payload);

    if (httpResponseCode == 200) {
        String response = http.getString();
        http.end();
        Serial.println(response);

        // 解析JSON响应
        DynamicJsonDocument jsonDoc(1024);
        deserializeJson(jsonDoc, response);
        String outputText = jsonDoc["data"]["choices"][0]["content"];
        return outputText;
    } else {
        http.end();
        Serial.printf("错误 %i \n", httpResponseCode);
        return "<error>";
    }
}

void setup() {
  pinMode(2,OUTPUT);
  Serial.begin(115200);
  Serial.println();
 
  WiFi.begin(ssid);
 
  http.begin(http_get_com); // 准备启用连接
 
  http.collectHeaders(headerKeys, 2); // 准备需要接收的响应头内容
 
  int httpCode = http.GET(); // 发起GET请求
 
  if (httpCode > 0) // 如果状态码大于0说明请求过程无异常
  {
    if (httpCode == HTTP_CODE_OK) // 请求被服务器正常响应，等同于httpCode == 200
    {
      Serial.print("Content-Type = ");
      Serial.println(http.header("Content-Type"));
 
      Serial.print("Content-Length = ");
      Serial.println(http.header("Content-Length"));
    }
  }
  else
  {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
 
  http.end(); // 结束当前连接

  answer = getGPTAnswer(inputText);
  Serial.println("回答: " + answer);
  Serial.println("输入提示:");
}

void loop() {
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
 
    while (WiFi.status() == WL_CONNECTED)
  {
    digitalWrite(2,LOW);
    delay(1000);
    digitalWrite(2,HIGH);
    delay(1000);
  }
    // 无操作
    if (Serial.available()) {
        inputText = Serial.readStringUntil('\n');
        inputText.trim();
        Serial.println("\n输入: " + inputText);
        answer = getGPTAnswer(inputText);
        Serial.println("回答: " + answer);
        Serial.println("输入提示:");
    }
    // delay(2);
}
