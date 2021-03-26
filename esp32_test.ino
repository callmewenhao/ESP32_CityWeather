/* 
 * 这是一个非常简单的ESP32项目
 * 功能：查询上海的天气情况
 */
#include "WiFi.h"
#include "HTTPClient.h"
#include "ArduinoJson.h"
#include "SSD1306.h"
#include "Wire.h"

const char *ssid = "17D3012";                      //wifi名
const char *password = "515522321";              //wifi密码
const char *host = "https://api.seniverse.com"; //心知天气APIhost
const char *apiKey = "S5Z6TKOCrnZtNe7gr";       //API key 私钥
const char *city = "shanghai";                     //查询的城市

int http_code = 0;
int time_flag = 0;

String req;
String rsp;
String city_name;
String city_weather;
String city_temperature;

//HTTPClint对象
HTTPClient http_client;
//JSON对象
StaticJsonDocument<512> doc;
//oled对象
SSD1306 oled(0x3c, 26, 25);
// 定时器指针
hw_timer_t * timer = NULL;
// timer interruption function
void IRAM_ATTR onTimer(){
  // Increment the counter and set the time of ISR
  if (time_flag <= 4)
  {
    /* code */
    time_flag++;
  } else
  time_flag = 0;
  
}

void setup()
{
  //OLED
  oled.init();
  oled.setFont(ArialMT_Plain_16);
  oled.drawString(0, 16, "your city weather!");
  oled.display();
  oled.clear();
  
  //串口
  Serial.begin(115200);
  delay(3000);
  
  //WiFi
  delay(10);
  Serial.println("connecting WIFI");
  WiFi.begin(ssid, password);
  while (!WiFi.isConnected())
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("OK");
  Serial.println("Wifi connected");
  
  //待访问的API https://api.seniverse.com/v3/weather/now.json?key=S5Z6TKOCrnZtNe7gr&location=shanghai&language=en&unit=c
  req = (String)host + "/v3/weather/now.json?key=";
  req += apiKey;
  req += "&location=";
  req += city;
  req += "&language=en&unit=c";
  Serial.println(req);
  if (http_client.begin(req))
  {
    Serial.println("HTTPclient setUp done!");
  }
  // 定时中断
  // Use 1st timer of 4 (counted from zero).
  // Set 80 divider for prescaler (see ESP32 Technical Reference Manual for more
  // info).
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  // // Set alarm to call onTimer function every second (value in microseconds).
  // // Repeat the alarm (third parameter)
  timerAlarmWrite(timer, 1000000, true); // 10 second T
  // // Start an alarm
  timerAlarmEnable(timer);
}

void loop()
{
  if (time_flag == 5)
  {
    http_code = http_client.GET();
    if (http_code > 0)
    {
      Serial.printf("HTTP get code: %d\n", http_code);
      if (http_code == HTTP_CODE_OK)
      {
        rsp = http_client.getString();
        // 解析JSON
        DeserializationError error = deserializeJson(doc, rsp);
        if (error) {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.f_str());
          return;
        }
        JsonObject results_0 = doc["results"][0];
        JsonObject results_0_location = results_0["location"];
        const char* results_0_location_name = results_0_location["name"]; // "上海"
    
        JsonObject results_0_now = results_0["now"];
        const char* results_0_now_text = results_0_now["text"];
        const char* results_0_now_temperature = results_0_now["temperature"];
        // 打印天气
        oled.drawString(0, 0, results_0_location_name);
        oled.drawString(0, 16, results_0_now_text);
        oled.drawString(65, 16, results_0_now_temperature);
        oled.drawString(85, 16, "°C");
        oled.display();

        // Serial.println(results_0_location_name);
        // Serial.println(results_0_now_text);
        // Serial.println(results_0_now_temperature);
      }
      else
      {
        Serial.printf("fail to get cityWeather,code:%d\n", http_code);
      }
    }
  }
}
