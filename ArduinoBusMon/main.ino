
void setup()
{
  tft.initR(INITR_BLACKTAB); // initialize a ST7735S chip, black tab
  tft.setRotation(3);
  tft.fillScreen(ST7735_BLACK);
  drawtext(2, 2, "H.U.G.O\nis here", ST7735_YELLOW, 1);
  
  Serial.begin(115200);
  Serial.setTimeout(5000);

  softSerial.begin(57600);

  while(!connectModule())
  {
    delay(1000);
  }

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
      delay(2000);
    }
  }
}

void loop()
{
  send("GET /json HTTP/1.1\r\nHost: busmon.azurewebsites.net\r\nConnection: close\r\n\r\n");
  delay(10000);
}

