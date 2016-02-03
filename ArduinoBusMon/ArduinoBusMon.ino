#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include "secret.h"

#define USE_SOFT_SERIAL
#ifdef USE_SOFT_SERIAL
#include <SoftwareSerial.h>
SoftwareSerial softSerial(6, 7);
#endif

#define PIR_PIN    3
#define TFT_CS     10
#define TFT_RST    8
#define TFT_DC     9
#define MAX_DEPARTURES 4
#define JSON_BUFFER_SIZE 200
#define PIR_INIT_SECONDS 25
#define PIR_PAUSE 600000L
//#define HOST "busmon.azurewebsites.net"
#define HOST "192.168.1.121"
#define PORT "4000"

Adafruit_ST7735 tft(TFT_CS,  TFT_DC, TFT_RST);
char json[JSON_BUFFER_SIZE];

unsigned long lastMotionTime = 0;
bool motion = false;

void setup()
{
  randomSeed(analogRead(0));
  tft.initR(INITR_BLACKTAB); // initialize a ST7735S chip, black tab
  tft.setRotation(3);
  clearScreen();
  drawtext(2, 2, "H.U.G.O\n (c) z1c0 2016", ST7735_YELLOW, 1);
  //
  // PIR setup
  //
  pinMode(PIR_PIN, INPUT);
  digitalWrite(PIR_PIN, LOW);
  // Give the sensor some time to calibrate
  for (int i = 0; i < PIR_INIT_SECONDS; i++)
  {
    drawtext(2 + i * 5, 22, ".", ST7735_YELLOW, 1);
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

  Serial.begin(115200);
  Serial.setTimeout(5000);

#ifdef USE_SOFT_SERIAL
  softSerial.begin(57600);
#endif

  initESP8266();
}

void loop()
{
  int errorCount = 0;
  unsigned long now = millis();

  if (digitalRead(PIR_PIN) == HIGH)
  {
    if (!motion)
    {
      // makes sure we wait for a transition to LOW before any further output is made:
      motion = true;
      trace("motion detected");
      tft.wake();
      tft.fillRect(0, 0, 5, 5, ST7735_YELLOW);
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
      tft.sleep();
    }
  }

  if (motion)
  {
    if (send("GET /busmon/json HTTP/1.1\r\nHost: "HOST"\r\nConnection: close\r\n\r\n"))
    {
      errorCount = 0;
    }
    else
    {
      errorCount++;
    }
  }
  if (errorCount > 10)
  {
    initESP8266();
    errorCount = 0;
  }
}


