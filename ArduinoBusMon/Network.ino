void parseHttpResponse()
{ 
  memset(busTimes, 0, MAX_BUSTIMES * sizeof(BusTime));
  currentLine = 0;  
  memset(json, 0, JSON_BUFFER_SIZE);
  json[0] = '{';
  if (Serial.readBytes(json + 1, JSON_BUFFER_SIZE - 1) > 0)    
  {
    trace(json);   
    
    StaticJsonBuffer<JSON_BUFFER_SIZE> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(json);
    if (root.success())
    {
      int i = 0;
      for (ListConstIterator<JsonPair> it = root.begin(); it != root.end(); ++it)
      {
        const char* line = it->key;
        busTimes[i].line = line;
        busTimes[i].direction = root[line]["to"];
        busTimes[i].minutes = root[line]["in"];
        i++;
      }
    }
  }
}

bool performCommand(const char* cmd, char* expect)
{
  trace("CMD: ");
  trace(cmd);
  
  Serial.println(cmd);
  //Serial.flush();
  delay(300);

  bool ret(true);
  if (expect)
  {
    ret = Serial.find(expect);
  }  
  displayTimes();
  return ret;
}

boolean connectModule()
{
  boolean ok = performCommand("AT+RST", "OK");
  if (ok)
  {
    performCommand("AT+GMR", "OK");
    //performCommand("AT+CIOBAUD?");
    delay(1000);
  }
  return ok;
}

boolean connectWiFi()
{
  performCommand("AT+CWMODE=1", NULL);
  bool ok = performCommand("AT+CWJAP=\""SSID"\",\""PASS"\"", "OK");
  if (ok)
  {
    performCommand("AT+CIFSR", "OK");
    performCommand("AT+CIPMUX=1", NULL);
  }
  return ok;
}

void initESP8266()
{
  tft.fillScreen(ST7735_BLACK);
  while (true)
  {
    tft.fillScreen(ST7735_BLACK);
    drawtext(0, 0, "connecting ESP8266...", ST7735_WHITE, 1);
    if (connectModule())
    {
      break;
    }
    drawtext(0, 10, "... failed", ST7735_RED, 1);      
    delay(1000);
  }
  
  // connect to WiFi
  while (true)
  {
    tft.fillScreen(ST7735_BLACK);
    drawtext(0, 0, "connecting WIFI ...", ST7735_WHITE, 1);
    if (connectWiFi())
    {
      break;
    }
    else
    {
      drawtext(0, 10, "... failed", ST7735_RED, 1);      
      delay(1000);
    }
  }
  tft.fillScreen(ST7735_BLACK);
}

bool send(const char* msg)
{
  bool ret = false;
  if (!performCommand("AT+CIPSTART=4,\"TCP\",\""HOST"\","PORT, "Error"))
  { 
    String s = "AT+CIPSEND=4,";
    s += strlen(msg);
    if (performCommand(s.c_str(), ">"))
    {
      if (performCommand(msg, "{"))
      {
        parseHttpResponse();
        ret = true;
      }
    }
    performCommand("AT+CIPCLOSE=4", NULL);
  }
  return ret;
}
