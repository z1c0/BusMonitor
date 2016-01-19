
void drawtext(int x, int y, const char *text, uint16_t color, int size)
{
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextSize(size);
  tft.setTextWrap(true);
  tft.print(text);
}

void trace(const char* message)
{
  //tft.fillScreen(ST7735_BLACK);
  //drawtext(0, 0, message, ST7735_WHITE, 1);
#ifdef USE_SOFT_SERIAL
  softSerial.println(message);
#endif
}

void displayTimes()
{
  unsigned long now = millis();
  if (now - lastRenderTime > SHOW_DURATION)
  {  
    lastRenderTime = now;

    trace("*** displayTimes ***");    
    if (currentLine < MAX_BUSTIMES && busTimes[currentLine].line != NULL)
    {
      trace(busTimes[currentLine].direction);
      
      tft.fillScreen(ST7735_BLACK);
      drawtext(110, 10, busTimes[currentLine].line, ST7735_GREEN, 4);
      drawtext(5, 35, busTimes[currentLine].direction, ST7735_GREEN, 1);
      drawtext(0, 70, busTimes[currentLine].minutes, ST7735_BLUE, 7);
      drawtext(75, 105, "Minuten", ST7735_GREEN, 2);
      currentLine++;
    }
    else if (busTimes[0].line != NULL)
    {
      trace("overview");
      
      // show overview  
      tft.fillScreen(ST7735_BLACK);
      int y = 5;
      for (int i = 0; i < MAX_BUSTIMES; i++)
      {
        if (busTimes[i].line != NULL)
        {
          drawtext(5, y, busTimes[i].line, ST7735_GREEN, 1);
          drawtext(25, y, busTimes[i].direction, ST7735_GREEN, 1);
          drawtext(5, y + 10, busTimes[i].minutes, ST7735_BLUE, 1);
          drawtext(25, y + 10, "Minuten", ST7735_GREEN, 1);
          y += 30;
        }
      }
      currentLine = 0;
    }
    else
    {
      trace("no bus data :(");
    }
  }
}
