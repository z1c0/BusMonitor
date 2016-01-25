
void drawtext(int x, int y, const char *text, uint16_t color, int size)
{
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextSize(size);
  tft.setTextWrap(true);
  tft.print(text);
}

void clearScreen()
{
  tft.fillScreen(ST7735_BLACK);
}

void trace(const char* message)
{
  //tft.fillScreen(ST7735_BLACK);
  //drawtext(0, 0, message, ST7735_WHITE, 1);
#ifdef USE_SOFT_SERIAL
  softSerial.println(message);
#endif
}

void displayTimes(bool showNoData)
{
  unsigned long now = millis();
  //
  // old bus data?
  //
  unsigned long diff = now - busTimes.lastUpdate;
  if (diff  > 1000 * 60)
  {
    int color = ST7735_YELLOW;
    if (diff > 1000 * 60 * 2)
    {
      color = ST7735_RED;
    }
    drawtext(tft.width() - 10, 0, "!", color, 1);    
    if (diff > 1000 * 60 * 5)
    {
      busTimes.clear();
    }
  }
  
  if (now - lastRenderTime > SHOW_DURATION)
  {  
    lastRenderTime = now;

    trace("*** displayTimes ***");    
    if (currentLine < MAX_DEPARTURES && busTimes.departures[currentLine].line != NULL)
    {
      trace(busTimes.departures[currentLine].direction);
      
      clearScreen();
      drawtext(110, 10, busTimes.departures[currentLine].line, ST7735_GREEN, 4);
      drawtext(5, 35, busTimes.departures[currentLine].direction, ST7735_GREEN, 1);
      drawtext(0, 70, busTimes.departures[currentLine].minutes, ST7735_BLUE, 7);
      drawtext(75, 105, "Minuten", ST7735_GREEN, 2);
      currentLine++;
    }
    else if (busTimes.departures[0].line != NULL)
    {
      trace("overview");
      
      // show overview  
      clearScreen();
      int y = 5;
      for (int i = 0; i < MAX_DEPARTURES; i++)
      {
        if (busTimes.departures[i].line != NULL)
        {
          drawtext(5, y, busTimes.departures[i].line, ST7735_GREEN, 1);
          drawtext(25, y, busTimes.departures[i].direction, ST7735_GREEN, 1);
          drawtext(5, y + 10, busTimes.departures[i].minutes, ST7735_BLUE, 1);
          drawtext(25, y + 10, "Minuten", ST7735_GREEN, 1);
          y += 30;
        }
      }
      currentLine = 0;
    }
    else if (showNoData)
    {      
      clearScreen();      
      drawtext(random(40), random(40), ":-/", ST7735_GREEN, 2);
    }
  }
}
