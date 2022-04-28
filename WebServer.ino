#include <dummy.h>

#include <Adafruit_SSD1306.h>

#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include "EEPROM.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 32

#define potPin 34
#define EEPROM_SIZE 509
const char* ssid = "ponza";
const char* password = "12345678";
IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
WebServer server(80);
bool LEDstatus = LOW;
const int ledPin = 25; 
const int freq = 5000; 
const int ledChannel = 0; 
const int resolution = 8; 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
String ip;
void IRAM_ATTR setDisplay();
void setup() {
 Serial.begin(115200);
 pinMode(ledPin, OUTPUT);
 WiFi.softAP(ssid, password);
 WiFi.softAPConfig(local_ip, gateway, subnet);
 delay(1000);
 server.on("/", handle_OnConnect);
 server.on("/ledon", handle_ledon);
 server.on("/ledoff", handle_ledoff);
 server.onNotFound(handle_NotFound);
 server.begin();
 Serial.println("HTTP server started");
ip=String(WiFi.softAPIP()[0])+"."+String(WiFi.softAPIP()[1])+"."+String(WiFi.softAPIP()[2])+"."+String(WiFi.softAPIP()[3]);
if (!EEPROM.begin(EEPROM_SIZE)) {
    delay(1000);
    ESP.restart();
 }
 if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
 }
 display.display();
}


void loop() {
 server.handleClient();
 if (LEDstatus)
 {
  ledcWrite(ledChannel, EEPROM.readUChar(460)); 
 }
 else
 {
  ledcWrite(ledChannel, 0); 
 }
 setDisplay();
 
}

void handle_OnConnect() {
 LEDstatus = LOW;
 Serial.println("GPIO25 Status: OFF ");
 server.send(200, "text/html", SendHTML(false));
}
void handle_ledon() {
 LEDstatus = HIGH;
 Serial.println("GPIO25 Status: ON");
 server.send(200, "text/html", SendHTML(true));
}
void handle_ledoff() {
 LEDstatus = LOW;
 Serial.println("GPIO25 Status: OFF");
 server.send(200, "text/html", SendHTML(false));
}
void handle_NotFound() {
 server.send(404, "text/plain", "Not found");
}

String SendHTML(uint8_t ledstat) {
 String ptr = "<!DOCTYPE html> <html>\n";
 ptr += "<head><meta name=\"viewport\"content=\"width=device-width, initial-scale=1.0, userscalable=no\">\n";
 ptr += "<title>LED Control</title>\n";
 ptr += "<style>html { font-family: Helvetica; display:inline-block; margin: 0px auto; text-align: center;}\n";
 ptr += "body{margin-top: 50px;backgroundcolor: #e9c46a;} h1 {color:#444444;margin: 50px auto 30px;} h3 {color:#444444;margin-bottom: 50px;}\n";
 ptr += ".button {display: block;width: 80px;backgroundcolor: #264653;border: none;color: white;padding: 13px ;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
 ptr += "p {font-size: 14px;color: #888;margin-bottom:10px;}\n";
 ptr += "</style>\n";
 ptr += "</head>\n";
 if (ledstat)
  ptr += "<body style=\"background-color:#ffb703;\" >\n";
 else
  ptr += "<body>\n";
 ptr += "<h1>ESP32 Web Server</h1>\n";
 ptr += "<h1>AP Mode</h2>\n";
 ptr += "<h1>IP :"+ip+"</h2>\n";
 ptr += "<h1>PWM Data :"+String(EEPROM.readUChar(508))+"</h2>\n";
 ptr += "<h1>LED Control</h2>\n";
 if (ledstat)
 {
  ptr += "<button class=\"button\" onclick=\"window.location.href='/ledoff'\" style=\"background-color:#264653;\" >OFF</button>\n";
  //ptr += "<p>LED1 Status: ON</p><a class=\"buttonbutton-off\" href=\"/ledoff\">OFF</a>\n";
 }
 else
 {
  ptr += "<button class=\"button\" onclick=\"window.location.href='/ledon'\" style=\"background-color:#264653;color:#fcbf49;\" >ON</button>\n";
  //ptr += "<p>LED1 Status: OFF</p><a class=\"buttonbutton-on\" href=\"/ledon\">ON</a>\n";
 }
 ptr += "</body>\n";
 ptr += "</html>\n";
 return ptr;
}
