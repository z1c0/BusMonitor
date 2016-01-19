#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include "secret.h"
using namespace ArduinoJson::Internals;

#define USE_SOFT_SERIAL
#ifdef USE_SOFT_SERIAL
#include <SoftwareSerial.h>
SoftwareSerial softSerial(6, 7);
#endif

#define PIR_PIN    3
#define TFT_CS     10
#define TFT_RST    8
#define TFT_DC     9
#define MAX_BUSTIMES 4
#define SHOW_DURATION 1500
#define JSON_BUFFER_SIZE 200
#define PIR_INIT_SECONDS 10
#define PIR_PAUSE 600000L
//#define HOST "busmon.azurewebsites.net"
#define HOST "192.168.1.121"
#define PORT "3000"

Adafruit_ST7735 tft(TFT_CS,  TFT_DC, TFT_RST);
char json[JSON_BUFFER_SIZE];

struct BusTime
{
  const char* line;
  const char* direction;
  const char* minutes;
};
BusTime busTimes[MAX_BUSTIMES] = {0};
int currentLine = 0;
unsigned long lastRequestTime = 0;
unsigned long lastRenderTime = 0;
unsigned long lastMotionTime = 0;
bool motion = false;


void setup()
{
  tft.initR(INITR_BLACKTAB); // initialize a ST7735S chip, black tab
  tft.setRotation(1);
  tft.fillScreen(ST7735_BLACK);
  drawtext(2, 2, "H.U.G.O\n(c) Wolfgang Ziegler 2016", ST7735_YELLOW, 1);
  //
  // PIR setup
  //
  pinMode(PIR_PIN, INPUT);
  digitalWrite(PIR_PIN, LOW);
  // Give the sensor some time to calibrate
  for (int i = 0; i < PIR_INIT_SECONDS; i++)
  {
    drawtext(2 + i * 10, 22, ".", ST7735_YELLOW, 1);
    delay(1000);
  }
  drawtext(2, 32, "OK READY", ST7735_YELLOW, 1);
  for (int i = 0; i < 5; i++)
  {
    tft.invertDisplay(true);
    delay(100);
    tft.invertDisplay(false);
    delay(100);
  }
  tft.fillScreen(ST7735_BLACK);

  Serial.begin(115200);
  Serial.setTimeout(5000);

#ifdef USE_SOFT_SERIAL
  softSerial.begin(57600);
#endif

  tft.fillRect(0, 0, 3, 3, ST7735_GREEN);
  while (!connectModule())
  {
    delay(1000);
  }
  tft.fillRect(0, 0, 3, 3, ST7735_RED);
  // connect to WiFi
  while (true)
  {
    if (connectWiFi())
    {
      break;
    }
    else
    {
      error("wireless connection");
      delay(1000);
    }
  }
}

void loop()
{
  unsigned long now = millis();

  if (digitalRead(PIR_PIN) == HIGH)
  {
    tft.fillRect(0, 0, 3, 3, ST7735_YELLOW);
    if (!motion)
    {
      // makes sure we wait for a transition to LOW before any further output is made:
      motion = true;
      trace("motion detected");
    }
    lastMotionTime = now;
  }
  if (digitalRead(PIR_PIN) == LOW)
  {
    // if the sensor is low for more than the given pause, we assume that no more motion is going to happen
    if (motion && (now - lastMotionTime) > PIR_PAUSE)
    {
      // makes sure this block of code is only executed again after a new motion sequence has been detected
      motion = false;
      trace("motion ended");
    }
  }

  if (motion && now - lastRequestTime > 15000)
  {    
    lastRequestTime = now;
    send("GET /json HTTP/1.1\r\nHost: "HOST"\r\nConnection: close\r\n\r\n");
  }
  
  displayTimes();
}


