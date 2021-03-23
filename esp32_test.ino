/* 
 * 这是一个非常简单的ESP32项目
 * 功能：查询上海的天气情况
 */



#include <Arduino.h>
#include "WiFi.h"
#include "HTTPClient.h"
#include "ArduinoJson.h"

const char *ssid = "17D3012";                      //wifi名
const char *password = "515522321";              //wifi密码
const char *host = "https://api.seniverse.com"; //心知天气APIhost
const char *apiKey = "S5Z6TKOCrnZtNe7gr";       //API key 私钥
const char *city = "shanghai";                     //查询的城市

String req;
String rsp;
HTTPClient http_client;
String city_name;
String city_weather;
String city_temperature;

//
StaticJsonDocument<512> doc;

void setup()
{
  
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
  
  //HTTPClint
  req = (String)host + "/v3/weather/now.json?key=";
  req += apiKey;
  req += "&location=";
  req += city;
  req += "&language=zh-Hans&unit=c";
  Serial.println(req);
  if (http_client.begin(req))
  {
    Serial.println("HTTPclient setUp done!");
  }
}

void loop()
{
  int http_code = http_client.GET();
  //Serial.println(http_code);
  if (http_code > 0)
  {
    //Serial.printf("HTTP get code: %d\n", http_code);
    if (http_code == HTTP_CODE_OK)
    {
      rsp = http_client.getString();
      
      DeserializationError error = deserializeJson(doc, rsp);
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }
      JsonObject results_0 = doc["results"][0];
      JsonObject results_0_location = results_0["location"];
//      const char* results_0_location_id = results_0_location["id"]; // "WTW3SJ5ZBJUY"
      const char* results_0_location_name = results_0_location["name"]; // "上海"
//      const char* results_0_location_country = results_0_location["country"]; // "CN"
//      const char* results_0_location_path = results_0_location["path"]; // "上海,上海,中国"
//      const char* results_0_location_timezone = results_0_location["timezone"]; // "Asia/Shanghai"
//      const char* results_0_location_timezone_offset = results_0_location["timezone_offset"]; // "+08:00"
//      
      JsonObject results_0_now = results_0["now"];
      const char* results_0_now_text = results_0_now["text"]; // "阴"
//      const char* results_0_now_code = results_0_now["code"]; // "9"
      const char* results_0_now_temperature = results_0_now["temperature"]; // "16"

      Serial.println(results_0_location_name);
      Serial.println(results_0_now_text);
      Serial.println(results_0_now_temperature);
      //Serial.println(city_temperature.c_str());
    }
    else
    {
      Serial.printf("fail to get cityWeather,code:%d\n", http_code);
    }
  }
  delay(5000);
}
