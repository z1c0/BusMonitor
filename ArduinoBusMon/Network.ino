bool performCommand(const char* cmd, bool parseHttpResponse = false)
{
  bool ret = false;
  
  trace("CMD: ");
  trace(cmd);
  
  Serial.println(cmd);
  Serial.flush();

  delay(300);

  if (parseHttpResponse)
  {
    memset(busTimes, 0, MAX_BUSTIMES * sizeof(BusTime));
    currentLine = 0;
    
    if (Serial.find("{"))
    {
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
        ret = true;
      }
    }
  }
  else
  {
    ret = Serial.find("OK");
  }
  displayTimes();
  return ret;
}

boolean connectModule()
{
  trace("*** Connect Module ***");
  
  boolean ok = performCommand("AT+RST");
  if (ok)
  {
    performCommand("AT+GMR");
    performCommand("AT+CIOBAUD?");
    performCommand("AT+CWMODE=1");
  }
  return ok;
}

boolean connectWiFi()
{
  trace("connectWiFi");

  bool ok = performCommand("AT+CWJAP=\""SSID"\",\""PASS"\"");
  if (ok)
  {
    performCommand("AT+CIFSR");
    performCommand("AT+CIPMUX=1");
  }
  return ok;
}

void send(const char* msg)
{
  // specify connection channel and IP
  performCommand("AT+CIPSTART=4,\"TCP\",\""HOST"\","PORT);
  /*
  if (Serial.find("ERROR"))
  {
    error("send error");
    return;
  }
  */
  String s = "AT+CIPSEND=4,";
  s += strlen(msg);
  performCommand(s.c_str());
  performCommand(msg, true);

  performCommand("AT+CIPCLOSE=4");
}
