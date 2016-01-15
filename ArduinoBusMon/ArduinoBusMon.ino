#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include "secret.h"
using namespace ArduinoJson::Internals;

#define TFT_CS     10
#define TFT_RST    8
#define TFT_DC     9

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);
SoftwareSerial softSerial(6, 7);



void drawtext(int x, int y, const char *text, uint16_t color, int size)
{
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextSize(size);
  tft.setTextWrap(true);
  tft.print(text);
}

void error(const char* message)
{
  tft.fillScreen(ST7735_BLACK);
  drawtext(0, 0, message, ST7735_RED, 2);
}

void trace(const char* message)
{
  //tft.fillScreen(ST7735_BLACK);
  //drawtext(0, 0, message, ST7735_WHITE, 1);
  softSerial.println(message);
}

void displayTimes(const JsonObject& root)
{
  for (ListConstIterator<JsonPair> it = root.begin(); it != root.end(); ++it)
  {
    const char* line = it->key;
    const char* dir = root[line]["direction"];
    int minutes = root[line]["minutes"];
    softSerial.println(line);
    softSerial.println(dir);
    softSerial.println(minutes);
    softSerial.println("----------------------");

    tft.fillScreen(ST7735_BLACK);
    drawtext(110, 10, line, ST7735_GREEN, 4);
    drawtext(5, 35, dir, ST7735_GREEN, 1);
    drawtext(0, 70, String(minutes).c_str(), ST7735_BLUE, 7);
    drawtext(75, 105, "Minuten", ST7735_GREEN, 2);

    delay(2000);
  }
}

boolean performCommand(const char* cmd, bool parseHttpResponse = false)
{
  trace("CMD: ");
  trace(cmd);
  Serial.println(cmd);
  Serial.flush();

  delay(300);

  String s = Serial.readString();
  trace("RESP >>>>");
  trace(s.c_str());
  //trace("RESP <<<<");

  if (parseHttpResponse)
  {
    int pos = s.indexOf('{');
    int pos2 = s.lastIndexOf('}');
    softSerial.println(pos);
    softSerial.println(pos2);
    if (pos >= 0)
    {
      s[pos2 + 1] = 0;
      trace(&s[pos]);
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(&s[pos]);
      if (root.success())
      {
        displayTimes(root);
      }
    }
  }
  return true;
}

boolean connectModule()
{
  softSerial.println("connectModule");

  /*
  Serial.println("AT+RST");
  if (Serial.find("ready"))
  {
    softSerial.println("ready");
  }
  else
  {
    softSerial.println("not ready!");
    return false;
  }
  */
  performCommand("AT+RST");
  performCommand("AT+GMR");
  performCommand("AT+CIOBAUD?");
  performCommand("AT+CWMODE=1");

  return true;
}
boolean connectWiFi()
{
  trace("connectWiFi");
  String cmd = "AT+CWJAP=\"";
  cmd += SSID;
  cmd += "\",\"";
  cmd += PASS;
  cmd += "\"";

  Serial.println(cmd);

  delay(1000);

  if (Serial.find("OK"))
  {
    performCommand("AT+CIFSR");
    performCommand("AT+CIPMUX=1");
    return true;
  }
  else
  {
    return false;
  }
}

void send(String msg)
{
  // specify connection channel and IP
  String cmd = "AT+CIPSTART=4,\"TCP\",\"";
  cmd += "busmon.azurewebsites.net";
  cmd += "\",80";
  Serial.println(cmd);
  if (Serial.find("ERROR"))
  {
    error("send error");
    return;
  }

  String s = "AT+CIPSEND=4,";
  s += msg.length();
  performCommand(s.c_str());
  performCommand(msg.c_str(), true);

  performCommand("AT+CIPCLOSE=4");
}

